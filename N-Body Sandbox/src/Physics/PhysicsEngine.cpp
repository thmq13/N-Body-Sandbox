#include <Physics/PhysicsEngine.hpp>

#include <Core/ApplicationState.hpp>
#include <Core/Logger.hpp>
#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Particle/ParticleBuffer.hpp>
#include <Physics/Gravity Solvers/DirectCPU.hpp>
#include <Physics/GravitySolver.hpp>
#include <Physics/Integrator.hpp>
#include <Physics/Integrators/Verlet.hpp>

import std;

namespace NBody::Physics {

    PhysicsEngine::PhysicsEngine(
        Core::MessageBus& messageBus,
        const std::atomic<Core::ApplicationState>& applicationState,
        std::shared_ptr<Particle::ParticleBuffer> particleBuffer
    ) noexcept
        : m_messageBus(messageBus),
        m_applicationState(applicationState),
        m_particleBuffer(std::move(particleBuffer)) {}

    PhysicsEngine::~PhysicsEngine() {
        if (m_workerThread.joinable()) {
            m_workerThread.request_stop();
            m_mailboxCV.notify_all();
        }
        NBODY_INFO("PHYSICS ENGINE: Deallocating resources.");
    }

    PhysicsEngine::Result PhysicsEngine::InitializeEngine() {
        if (m_isInitialized) {
            return std::unexpected(Core::PhysicsError::ErrAlreadyInitialized);
        }

        if (!m_particleBuffer) {
            return std::unexpected(Core::PhysicsError::ErrParticleBufferNull);
        }

        SubscribeToMessages<
            Core::CmdRequestSchemas,
            Core::CmdSetActiveOption,
            Core::CmdSetConfig,
            Core::CmdSendParticles
        >();

        RegisterGravitySolvers();
        if (!m_gravitySolvers.contains(m_currentGravitySolver)) {
            NBODY_ERROR("PHYSICS ENGINE: Default gravity solver '{}' not found.", m_currentGravitySolver);
            return std::unexpected(Core::PhysicsError::ErrSolverNotFound);
        }

        RegisterIntegrators();
        if (!m_integrators.contains(m_currentIntegrator)) {
            NBODY_ERROR("PHYSICS ENGINE: Default integrator '{}' not found.", m_currentIntegrator);
            return std::unexpected(Core::PhysicsError::ErrIntegratorNotFound);
        }
      
        try {
            m_workerThread = std::jthread([this](std::stop_token token) { WorkerLoop(token); });
        }
        catch (const std::system_error& e) {
            NBODY_ERROR("PHYSICS ENGINE: Worker thread spawn failed: {}", e.what());
            return std::unexpected(Core::PhysicsError::ErrWorkerThreadStartFailed);
        }

        m_isInitialized = true;

        NBODY_INFO("PHYSICS ENGINE: Engine initialized successfully.");
        return {};
    }

    void PhysicsEngine::ProcessMailbox() {
        std::queue<Core::SystemMessage> localMailbox;
        {
            std::lock_guard<std::mutex> lock(m_mailboxMutex);
            if (!m_mailbox.empty()) {
                std::swap(localMailbox, m_mailbox);
            }
        }

        while (!localMailbox.empty()) {
            HandleMessage(localMailbox.front());
            localMailbox.pop();
        }
    }

    void PhysicsEngine::HandleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& msg) {
            using MessageType = std::decay_t<decltype(msg)>;

            if constexpr (std::is_same_v<MessageType, Core::CmdRequestSchemas>) {
                SendSchemas();
            }
            else if constexpr (std::is_same_v<MessageType, Core::CmdSetActiveOption>) {
                if (static_cast<Core::Module>(msg.moduleId) != Core::Module::Physics) {
                    return;
                }
                auto setRes{ SetActiveOption(static_cast<Core::PhysicsSubModule>(msg.subModuleId), msg.activeId) };
                if (!setRes) {
                    NBODY_ERROR("PHYSICS ENGINE: SetActiveOption failed: {}", setRes.error());
                }
            }
            else if constexpr (std::is_same_v<MessageType, Core::CmdSetConfig>) {
                if (static_cast<Core::Module>(msg.moduleId) != Core::Module::Physics) {
                    return;
                }
                auto setRes{ SetConfig(static_cast<Core::PhysicsSubModule>(msg.subModuleId), msg.targetId, msg.schema) };
                if (!setRes) {
                    NBODY_ERROR("PHYSICS ENGINE: SetConfig failed: {}", setRes.error());
                }
            }
            else if constexpr (std::is_same_v<MessageType, Core::CmdSendParticles>) {
                if (msg.shouldAppend) {
                    m_simulationBuffer.append(std::move(msg.particleSystem));
                }
                else {
                    m_simulationBuffer = std::move(msg.particleSystem);
                }
                m_needSimulationBufferUpdate = true;

                NBODY_TRACE("PHYSICS ENGINE: Particle system payload ingested. Buffer size: {}", m_simulationBuffer.getSize());
            }
        }, message);
    }

    void PhysicsEngine::RegisterGravitySolvers() {
        m_gravitySolvers["Direct CPU"] = std::make_unique<DirectCPU>();
    }

    void PhysicsEngine::RegisterIntegrators() {
        m_integrators["Verlet"] = std::make_unique<Verlet>();
    }

    void PhysicsEngine::SendSchemas() {
        for (const auto& [solverId, solver] : m_gravitySolvers) {
            m_messageBus.publish(Core::CmdSendSchemas{
                static_cast<std::uint32_t>(Core::Module::Physics),
                static_cast<std::uint32_t>(Core::PhysicsSubModule::GravitySolver),
                solverId, solver->getSchemas()
            });
        }
        for (const auto& [integratorId, integrator] : m_integrators) {
            m_messageBus.publish(Core::CmdSendSchemas{
                static_cast<std::uint32_t>(Core::Module::Physics),
                static_cast<std::uint32_t>(Core::PhysicsSubModule::Integrator),
                integratorId, integrator->getSchemas()
            });
        }
        NBODY_INFO("PHYSICS ENGINE: Published schemas of components.");
    }

    PhysicsEngine::Result PhysicsEngine::SetConfig(Core::PhysicsSubModule subModule, const std::string& targetId,
        const Core::ParameterSchema& schema)
    {
        if (!m_isInitialized) {
            return std::unexpected(Core::PhysicsError::ErrNotInitialized);
        }

        if (subModule == Core::PhysicsSubModule::GravitySolver) {
            if (auto it = m_gravitySolvers.find(targetId); it != m_gravitySolvers.end()) {
                it->second->setParameter(schema);
            }
            else {
                NBODY_ERROR("PHYSICS ENGINE: Gravity solver '{}' not found.", targetId);
                return std::unexpected(Core::PhysicsError::ErrSolverNotFound);
            }
        }
        else if (subModule == Core::PhysicsSubModule::Integrator) {
            if (auto it = m_integrators.find(targetId); it != m_integrators.end()) {
                it->second->setParameter(schema);
            }
            else {
                NBODY_ERROR("PHYSICS ENGINE: Integrator '{}' not found.", targetId);
                return std::unexpected(Core::PhysicsError::ErrIntegratorNotFound);
            }
        }

        NBODY_TRACE("PHYSICS ENGINE: Changed configuration of '{}' with label '{}'.", targetId, schema.label);

        return{};
    }

    PhysicsEngine::Result PhysicsEngine::SetActiveOption(Core::PhysicsSubModule subModule, const std::string& activeId) {
        if (!m_isInitialized) {
            return std::unexpected(Core::PhysicsError::ErrNotInitialized);
        }

        if (subModule == Core::PhysicsSubModule::GravitySolver) {
            if (m_gravitySolvers.contains(activeId)) {
                m_currentGravitySolver = activeId;
                NBODY_TRACE("PHYSICS ENGINE: Set gravity solver option to '{}'.", activeId);
            }
            else {
                NBODY_ERROR("PHYSICS ENGINE: SetActiveOption failed: Gravity solver '{}' not found.", activeId);
                return std::unexpected(Core::PhysicsError::ErrSolverNotFound);
            }
        }
        else if (subModule == Core::PhysicsSubModule::Integrator) {
            if (m_integrators.contains(activeId)) {
                m_currentIntegrator = activeId;
                NBODY_TRACE("PHYSICS ENGINE: Set integrator option to '{}'.", activeId);
            }
            else {
                NBODY_ERROR("PHYSICS ENGINE: SetActiveOption failed: Integrator '{}' not found.", activeId);
                return std::unexpected(Core::PhysicsError::ErrIntegratorNotFound);
            }
        }

        return {};
    }

    void PhysicsEngine::UploadToBackBuffer() {
        auto& backBuffer = m_particleBuffer->getBackBuffer();
        backBuffer = m_simulationBuffer;
        m_particleBuffer->commitBackBuffer();
    }

    void PhysicsEngine::WorkerLoop(std::stop_token stopToken) {
        NBODY_INFO("PHYSICS ENGINE: Worker thread engaged.");

        while (!stopToken.stop_requested()) {
            ProcessMailbox();

            if (m_applicationState == Core::ApplicationState::RealTimeRunning ||
                m_applicationState == Core::ApplicationState::PrecomputeRunning)
            {
                // Execute physics step using active solver & integrator...

                UploadToBackBuffer();
            }
            else {
                if (m_needSimulationBufferUpdate) {
                    m_needSimulationBufferUpdate = false;
                    UploadToBackBuffer();
                }

                std::unique_lock<std::mutex> lock(m_mailboxMutex);
                m_mailboxCV.wait(lock, stopToken, [this]() {
                    return !m_mailbox.empty();
                });
            }
        }

        NBODY_INFO("PHYSICS ENGINE: Worker thread terminated cleanly.");
    }

} // namespace NBody::Physics