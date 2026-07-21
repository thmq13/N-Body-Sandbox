#include <Physics/PhysicsEngine.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <utility>
#include <variant>

#include <Core/AppState.hpp>
#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Particle/ParticleBuffer.hpp>
#include <Physics/Gravity Solvers/DirectCPU.hpp>
#include <Physics/GravitySolver.hpp>
#include <Physics/Integrator.hpp>
#include <Physics/Integrators/Verlet.hpp>

namespace NBody::Physics {
    PhysicsEngine::PhysicsEngine(Core::MessageBus& messageBus, std::shared_ptr<Particle::ParticleBuffer> particleBuffer)
        : m_messageBus(messageBus),
        m_particleBuffer(std::move(particleBuffer))
    {
        subscribeToMessages<
            Core::CmdExitApplication,
            Core::CmdRequestStateChange,

            Core::CmdRequestSchemas,
            Core::CmdSetActiveOption,
            Core::CmdSetConfig,

            Core::CmdSendParticles
        >();

        registerGravitySolvers();
        registerIntegrators();

        m_workerThread = std::jthread([this](std::stop_token token) { workerLoop(token); });
    }

    PhysicsEngine::~PhysicsEngine() {}
    
    void PhysicsEngine::processMailbox() {
        std::queue<Core::SystemMessage> localMailbox;
        {
            std::lock_guard<std::mutex> lock(m_mailboxMutex);
            if (!m_mailbox.empty()) {
                std::swap(localMailbox, m_mailbox);
            }
        }

        while (!localMailbox.empty()) {
            handleMessage(localMailbox.front());
            localMailbox.pop();
        }
    }

    void PhysicsEngine::handleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& msg) {
            using T = std::decay_t<decltype(msg)>;

            if constexpr (std::is_same_v<T, Core::CmdExitApplication>) {
                std::cout << "[Physics Engine] Exit command acknowledeged.\n";
                m_workerThread.request_stop();
            }

            else if constexpr (std::is_same_v<T, Core::CmdRequestStateChange>) {
                std::cout << "[Physics Engine] State sync acknowledeged.\n";
                m_appState = msg.requestedState;
            }

            else if constexpr (std::is_same_v<T, Core::CmdRequestSchemas>) {
                std::cout << "[Physics Engine] Schemas request intercepted.\n";
                sendSchemas();
            }

            else if constexpr (std::is_same_v<T, Core::CmdSetActiveOption>) {
                if (static_cast<Core::Module>(msg.moduleId) != Core::Module::Physics) {
                    return;
                }
                std::cout << "[Physics Engine] Active option acknowledged.\n";
                setActiveOption(msg.subModuleId, msg.activeId);
            }

            else if constexpr (std::is_same_v<T, Core::CmdSetConfig>) {
                if (static_cast<Core::Module>(msg.moduleId) != Core::Module::Physics) {
                    return;
                }
                std::cout << "[Physics Engine] Configuration change acknowledged.\n";
                setConfig(msg.subModuleId, msg.targetId, msg.schemas);
            }   

            else if constexpr (std::is_same_v<T, Core::CmdSendParticles>) {
                if (msg.shouldAppend) {
                    m_simulationBuffer.append(std::move(msg.particleSystem));
                }
                else {
                    m_simulationBuffer = std::move(msg.particleSystem);
                }
                m_needSimulationBufferUpdate = true;

                std::cout << "[Physics Engine] Acknowledeged new particles. The current local physics buffer size is "
                    << m_simulationBuffer.getSize() << '\n';
            }
            }, message);
    }

    void PhysicsEngine::registerGravitySolvers() {
        m_gravitySolvers["Direct CPU"] = std::make_unique<DirectCPU>();
    }

    void PhysicsEngine::registerIntegrators() {
        m_integrators["Verlet"] = std::make_unique<Verlet>();
    }

    void PhysicsEngine::sendSchemas() {
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
    }

    void PhysicsEngine::setConfig(std::uint32_t subModuleId, const std::string& targetId,
        const std::vector<Core::ParameterSchema>& schemas)
    {
        auto subModule = static_cast<Core::PhysicsSubModule>(subModuleId);
        if (subModule == Core::PhysicsSubModule::GravitySolver) {
            m_gravitySolvers[targetId]->setParameters(schemas);
        }
        if (subModule == Core::PhysicsSubModule::Integrator) {
            m_integrators[targetId]->setParameters(schemas);
        }
    }

    void PhysicsEngine::setActiveOption(std::uint32_t subModuleId, const std::string& activeId) {
        auto subModule = static_cast<Core::PhysicsSubModule>(subModuleId);
        if (subModule == Core::PhysicsSubModule::GravitySolver) {
            m_currentGravitySolver = activeId;
        }
        if (subModule == Core::PhysicsSubModule::Integrator) {
            m_currentIntegrator = activeId;
        }
    }

    void PhysicsEngine::workerLoop(std::stop_token stopToken) {
        std::cout << "[Physics Engine] Worker thread started.\n";

        while (!stopToken.stop_requested()) {
            processMailbox();

            if (m_appState == Core::AppState::RealTimeRunning ||
                m_appState == Core::AppState::PrecomputeRunning)
            {
                // do physics ...

                uploadToBackBuffer();
            }
            else {
                if (m_needSimulationBufferUpdate) {
                    m_needSimulationBufferUpdate = false;
                    uploadToBackBuffer();
                }

                std::unique_lock<std::mutex> lock(m_mailboxMutex);
                m_mailboxCV.wait(lock, stopToken, [this]() {
                    return !m_mailbox.empty();
                });
            }
        }

        std::cout << "[Physics Engine] Worker thread spun down safely.\n";
    }

    void PhysicsEngine::uploadToBackBuffer() {
        auto& backBuffer = m_particleBuffer->getBackBuffer();
        backBuffer = m_simulationBuffer;
        m_particleBuffer->commitBackBuffer();
    }
}

