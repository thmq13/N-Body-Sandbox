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

namespace NBody::Particle { class ParticleBuffer; }

namespace NBody::Physics {
    class IGravitySolver;
    class IIntegrator;

    class PhysicsEngine {
    public:
        explicit PhysicsEngine(Core::MessageBus& messageBus, std::shared_ptr<Particle::ParticleBuffer> particleBuffer);

        ~PhysicsEngine();

        PhysicsEngine(const PhysicsEngine&) = delete;
        PhysicsEngine& operator=(const PhysicsEngine&) = delete;
        PhysicsEngine(PhysicsEngine&&) = delete;
        PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    private:
        std::queue<Core::SystemMessage> m_mailbox;
        std::mutex m_mailboxMutex;
        std::condition_variable m_mailboxCV;
        Core::MessageBus& m_messageBus;

        void processMailbox();
        void handleMessage(const Core::SystemMessage& message);

        template <typename T>
        void subscribeToMessage() {
            m_messageBus.template subscribe<T>([this](const Core::SystemMessage& message) {
                {
                    std::lock_guard<std::mutex> lock(m_mailboxMutex);
                    m_mailbox.push(message);
                }
                m_mailboxCV.notify_one();
            });
        }

        template <typename... MessageTypes>
        void subscribeToMessages() {
            (this->template subscribeToMessage<MessageTypes>(), ...);
        }

        std::unordered_map<std::string, std::unique_ptr<IGravitySolver>> m_gravitySolvers{};
        std::unordered_map<std::string, std::unique_ptr<IIntegrator>> m_integrators{};
        void registerGravitySolvers();
        void registerIntegrators();

        Core::AppState m_appState{ Core::AppState::TitleScreen };
        std::string m_currentGravitySolver{ "Direct CPU" };
        std::string m_currentIntegrator{ "Verlet" };
        std::shared_ptr<Particle::ParticleBuffer> m_particleBuffer{};
        Particle::ParticleSystem m_simulationBuffer{};
        bool m_needSimulationBufferUpdate{ true };

        std::jthread m_workerThread;
        void workerLoop(std::stop_token stopToken);
        void uploadToBackBuffer();
    };
}
