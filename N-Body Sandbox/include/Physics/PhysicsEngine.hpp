#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>   

#include <Core/Events.hpp>  
#include <AppState.hpp>
#include <Config.hpp>

class ISolver;
class IIntegrator;
class MessageBus;

class PhysicsEngine {
public:
    explicit PhysicsEngine(MessageBus& messageBus);
    
    ~PhysicsEngine() = default;

    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

private:
    //std::unique_ptr<ISolver> m_solver{};
    //std::unique_ptr<IIntegrator> m_integrator{};

    ComputeConfig m_computeConfig{};
    PhysicsConfig m_physicsConfig{};

    AppState m_appState{ AppState::TitleScreen };

    std::queue<SystemMessage> m_mailbox;
    std::mutex m_mailboxMutex;
    std::condition_variable m_mailboxCV;

    MessageBus& m_messageBus;

    std::jthread m_workerThread;

    void workerLoop(std::stop_token stopToken);

    void processMailbox();
    void handleMessage(const SystemMessage& message);

    void reinitializeComponents();
};