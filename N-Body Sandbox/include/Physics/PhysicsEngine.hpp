#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <condition_variable>   

#include <Core/Message.hpp>  
#include <Core/AppState.hpp>
#include <Core/MessageBus.hpp>
#include <Particle/Particle.hpp>
#include <Particle/ParticleBuffer.hpp>

class IGravitySolver;
class IIntegrator;



class PhysicsEngine {
public:
    explicit PhysicsEngine(MessageBus& messageBus, std::shared_ptr<ParticleBuffer> particleBuffer);
    
    ~PhysicsEngine();

    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

private:
    // Communication
    std::queue<SystemMessage> m_mailbox;
    std::mutex m_mailboxMutex;
    std::condition_variable m_mailboxCV;
    MessageBus& m_messageBus;

    void processMailbox();
    void handleMessage(const SystemMessage& message);

    template <typename T>
    void subscribeToMessage() {
        m_messageBus.template subscribe<T>([this](const SystemMessage& message) {
            {
                std::lock_guard<std::mutex> lock(m_mailboxMutex);
                m_mailbox.push(message);
            }
            m_mailboxCV.notify_one();
            });
    }

    template <typename... MsgTypes>
    void subscribeToMessages() {
        (this->template subscribeToMessage<MsgTypes>(), ...);
    }

    // Tools
    std::unordered_map<std::string, std::unique_ptr<IGravitySolver>> m_gravitySolvers{};
    std::unordered_map<std::string, std::unique_ptr<IIntegrator>> m_integrators{};
    void registerGravitySolvers();
    void registerIntegrators();

    // States
    AppState m_appState{ AppState::TitleScreen };
    std::string m_currentGravitySolver{ "Direct CPU" };
    std::string m_currentIntegrator{ "Verlet" };
    std::shared_ptr<ParticleBuffer> m_particleBuffer{};
    ParticleSystem m_simulationBuffer{};
    bool m_needsInitialization{ true };
        
    // Thread work
    std::jthread m_workerThread;
    void workerLoop(std::stop_token stopToken);
};