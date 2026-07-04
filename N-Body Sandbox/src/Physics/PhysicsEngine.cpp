#include <Physics/PhysicsEngine.hpp>

#include <iostream>

#include <Core/MessageBus.hpp>

PhysicsEngine::PhysicsEngine(MessageBus& messageBus)
    : m_messageBus(messageBus)

      //m_solver(std::make_unique)
{
    m_messageBus.subscribe<CmdUpdateSimulationConfig>([this](const SystemMessage& message) {
        std::lock_guard<std::mutex> lock(m_mailboxMutex);
        m_mailbox.push(message);
        m_mailboxCV.notify_one();
    });

    m_messageBus.subscribe<CmdRequestStateChange>([this](const SystemMessage& message) {
        std::lock_guard<std::mutex> lock(m_mailboxMutex);
        m_mailbox.push(message);
        m_mailboxCV.notify_one();
    });

    m_workerThread = std::jthread([this](std::stop_token token) { workerLoop(token); });
}

void PhysicsEngine::workerLoop(std::stop_token stopToken) {
    std::cout << "[Physics Engine] Worker thread started.\n";

    while (!stopToken.stop_requested()) {
        processMailbox();

        if (m_appState == AppState::RealTimeRunning ||
            m_appState == AppState::PrecomputeRunning)
        {
            // m_solver->();
            // m_integrator->();
        }
        else {
            std::unique_lock<std::mutex> lock(m_mailboxMutex);
            m_mailboxCV.wait_for(lock, std::chrono::milliseconds(10), [this]() {
                return !m_mailbox.empty();
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

        if constexpr (std::is_same_v<T, CmdUpdateSimulationConfig>) {
            m_computeConfig = actualMessage.computeConfig;
            m_physicsConfig = actualMessage.physicsConfig;
            std::cout << "[Physics Engine] Updated configuration simulation parameters.\n";

            std::cout << m_physicsConfig.GConstant << '\n';
            std::cout << m_physicsConfig.deltaTime << '\n';

        }
        else if constexpr (std::is_same_v<T, CmdRequestStateChange>) {
            m_appState = actualMessage.requestedState;
            std::cout << "[Physics Engine] State sync acknowledeged.\n";
        }
    }, message);
}

void PhysicsEngine::reinitializeComponents()
{
}
