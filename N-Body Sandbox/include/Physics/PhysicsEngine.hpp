#pragma once

#include <atomic>
#include <memory>
#include <expected>
#include <string>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <Core/ApplicationState.hpp>
#include <Core/Message.hpp>  
#include <Core/MessageBus.hpp>
#include <Core/Logger.hpp>
#include <Particle/Particle.hpp>

namespace NBody::Particle { class ParticleBuffer; }

namespace NBody::Physics {
    class GravitySolver;
    class Integrator;

    class PhysicsEngine final : public Core::MessageSubscriber<PhysicsEngine> {
    public:
        friend class Core::MessageSubscriber<PhysicsEngine>;

        using Result = std::expected<void, Core::PhysicsError>;

        explicit PhysicsEngine(
            Core::MessageBus& messageBus,
            const std::atomic<Core::ApplicationState>& applicationState,
            std::shared_ptr<Particle::ParticleBuffer> particleBuffer
        ) noexcept;

        ~PhysicsEngine();

        PhysicsEngine(const PhysicsEngine&) = delete;
        PhysicsEngine& operator=(const PhysicsEngine&) = delete;
        PhysicsEngine(PhysicsEngine&&) noexcept = delete;
        PhysicsEngine& operator=(PhysicsEngine&&) noexcept = delete;

        [[nodiscard]] Result InitializeEngine();

        void NotifyStateChanged() noexcept { m_mailboxCV.notify_all(); }

    private:
        bool m_isInitialized{ false };
        const std::atomic<Core::ApplicationState>& m_applicationState;

        Core::MessageBus&               m_messageBus;
        std::queue<Core::SystemMessage> m_mailbox;
        std::mutex                      m_mailboxMutex;
        std::condition_variable_any     m_mailboxCV;
        void ProcessMailbox();
        void HandleMessage(const Core::SystemMessage& message);
        template <typename MessageType>
        void SubscribeToMessage() {
            m_messageBus.template Subscribe<MessageType>([this](const Core::SystemMessage& message) {
                {
                    std::lock_guard<std::mutex> lock(m_mailboxMutex);
                    m_mailbox.push(message);
                }
                m_mailboxCV.notify_one();
            });
        }

        std::unordered_map<std::string, std::unique_ptr<GravitySolver>> m_gravitySolvers{};
        std::unordered_map<std::string, std::unique_ptr<Integrator>>    m_integrators{};
        std::string m_currentGravitySolver{ "Direct CPU" };
        std::string m_currentIntegrator{ "Verlet" };
        void RegisterGravitySolvers();
        void RegisterIntegrators();
        void SendSchemas();
        [[nodiscard]] Result SetConfig(Core::PhysicsSubModule subModule, const std::string& targetId, const Core::ParameterSchema& schema);
        [[nodiscard]] Result SetActiveOption(Core::PhysicsSubModule subModule, const std::string& activeId);

        std::shared_ptr<Particle::ParticleBuffer> m_particleBuffer{};
        Particle::ParticleSystem                  m_simulationBuffer{};
        bool m_needSimulationBufferUpdate{ true };
        void UploadToBackBuffer();

        std::jthread m_workerThread;        
        void WorkerLoop(std::stop_token stopToken);
    };
} // namespace NBody::Physics