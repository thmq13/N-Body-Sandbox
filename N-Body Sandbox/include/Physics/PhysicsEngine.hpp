#pragma once

#include <cstdint>
#include <condition_variable>   
#include <memory>
#include <mutex>
#include <queue>
#include <stop_token>
#include <string>
#include <thread>
#include <unordered_map>

#include <Core/AppState.hpp>
#include <Core/Message.hpp>  
#include <Core/MessageBus.hpp>
#include <Particle/Particle.hpp>

namespace NBody::Particle { class ParticleBuffer; }

namespace NBody::Physics {
    class GravitySolver;
    class Integrator;

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
        std::condition_variable_any m_mailboxCV;
        Core::MessageBus& m_messageBus;

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

        void processMailbox();
        void handleMessage(Core::SystemMessage& message);

        std::unordered_map<std::string, std::unique_ptr<GravitySolver>> m_gravitySolvers{};
        std::unordered_map<std::string, std::unique_ptr<Integrator>> m_integrators{};
        void registerGravitySolvers();
        void registerIntegrators();
        void sendSchemas();
        void setConfig(std::uint32_t subModuleId, const std::string& targetId,
            const std::vector<Core::ParameterSchema>& schemas);

        Core::AppState m_appState{ Core::AppState::TitleScreen };

        std::string m_currentGravitySolver{ "Direct CPU" };
        std::string m_currentIntegrator{ "Verlet" };
        void setActiveOption(std::uint32_t subModuleId, const std::string& activeId);

        std::shared_ptr<Particle::ParticleBuffer> m_particleBuffer{};
        Particle::ParticleSystem m_simulationBuffer{};
        bool m_needSimulationBufferUpdate{ true };

        std::jthread m_workerThread;
        void workerLoop(std::stop_token stopToken);
        void uploadToBackBuffer();
    };
}
