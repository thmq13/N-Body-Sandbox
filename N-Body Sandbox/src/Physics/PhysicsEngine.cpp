#include <Physics/PhysicsEngine.hpp>

#include <iostream>
#include <cstdint>

#include <Physics/IGravitySolver.hpp>
#include <Physics/Gravity Solvers/DirectCPU.hpp>
#include <Physics/IIntegrator.hpp>
#include <Physics/Integrators/Verlet.hpp>

PhysicsEngine::PhysicsEngine(MessageBus& messageBus, std::shared_ptr<ParticleBuffer> particleBuffer)
    : m_messageBus(messageBus),
      m_particleBuffer(std::move(particleBuffer))
{

    subscribeToMessages<
        CmdRequestStateChange,
        CmdRequestSchemas,
        CmdSetActiveOption,
        CmdSetConfig,
        CmdExitApplication
    >();

    m_workerThread = std::jthread([this](std::stop_token token) { workerLoop(token); });

    registerGravitySolvers();
    registerIntegrators();
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::workerLoop(std::stop_token stopToken) {
    std::cout << "[Physics Engine] Worker thread started.\n";

    while (!stopToken.stop_requested()) {
        processMailbox();

        if (m_appState == AppState::RealTimeRunning ||
            m_appState == AppState::PrecomputeRunning)
        {
            if (m_needsInitialization) {
                /*m_simulationBuffer = m_particleBuffer->getBackBuffer();*/
                m_needsInitialization = false;
                std::cout << "[Physics Engine] Initialized simulation state with "
                    << m_simulationBuffer.getSize() << " particles.\n";
            }

                
            m_particleBuffer->commitBackBuffer();
        }
        else {
            m_needsInitialization = true;

            std::unique_lock<std::mutex> lock(m_mailboxMutex);
            m_mailboxCV.wait(lock, [this, &stopToken]() {
                return stopToken.stop_requested() || !m_mailbox.empty();
            });
        }
    }

    std::cout << "[Physics Engine] Worker thread spun down safely.\n";
}

void PhysicsEngine::processMailbox() {
    std::queue<SystemMessage> localMailbox;

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

void PhysicsEngine::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;

        if constexpr (std::is_same_v<T, CmdRequestStateChange>) {
            std::cout << "[Physics Engine] State sync acknowledeged.\n";
            m_appState = actualMessage.requestedState;         
        }

        else if constexpr (std::is_same_v<T, CmdRequestSchemas>) {
            std::cout << "[Physics Engine] Schemas request intercepted.\n";

            for (const auto& [solverId, solver] : m_gravitySolvers) {
                m_messageBus.publish(CmdSendSchemas{ 
                    static_cast<std::uint32_t>(Module::Physics),
                    static_cast<std::uint32_t>(PhysicsSubModule::GravitySolver),
                    solverId, solver->getSchemas()
                });
            }
            for (const auto& [integratorId, integrator] : m_integrators) {
                m_messageBus.publish(CmdSendSchemas{ 
                    static_cast<std::uint32_t>(Module::Physics),
                    static_cast<std::uint32_t>(PhysicsSubModule::Integrator),
                    integratorId, integrator->getSchemas()
                });
            }
        }

        else if constexpr (std::is_same_v<T, CmdSetActiveOption>) {
            if (static_cast<Module>(actualMessage.moduleId) != Module::Physics) {
                return;
            }
            std::cout << "[Physics Engine] Active option acknowledged.\n";

            auto subModule = static_cast<PhysicsSubModule>(actualMessage.subModuleId);
            if (subModule == PhysicsSubModule::GravitySolver) {
                m_currentGravitySolver = actualMessage.activeId;
            }
            if (subModule == PhysicsSubModule::Integrator) {
                m_currentIntegrator = actualMessage.activeId;
            }
        }

        else if constexpr (std::is_same_v<T, CmdSetConfig>) {
            if (static_cast<Module>(actualMessage.moduleId) != Module::Physics) {
                return;
            }
            std::cout << "[Physics Engine] Configuration change acknowledged.\n";

            auto subModule = static_cast<PhysicsSubModule>(actualMessage.subModuleId);
            if (subModule == PhysicsSubModule::GravitySolver) {
                m_gravitySolvers[actualMessage.targetId]->setParameters(actualMessage.schemas);
            }
            if (subModule == PhysicsSubModule::Integrator) {
                m_integrators[actualMessage.targetId]->setParameters(actualMessage.schemas);
            }
        }

        else if constexpr (std::is_same_v<T, CmdExitApplication>) {
            std::cout << "[Physics Engine] Exit command acknowledeged.\n";
            m_workerThread.request_stop();
        }
    }, message);
}

void PhysicsEngine::registerGravitySolvers() {
    m_gravitySolvers["Direct CPU"] = std::make_unique<DirectCPU>();
}

void PhysicsEngine::registerIntegrators() {
    m_integrators["Verlet"] = std::make_unique<Verlet>();
}
