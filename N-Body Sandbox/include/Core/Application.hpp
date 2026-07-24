#pragma once

#include <memory>
#include <expected>
#include <atomic>

#include <Core/ApplicationState.hpp>
#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Core/Logger.hpp>

namespace NBody::Particle { class ParticleBuffer; }
namespace NBody::Generator { class GeneratorEngine; }
namespace NBody::Render { class RenderingEngine; }
namespace NBody::UI { class UIManager; }
namespace NBody::Physics { class PhysicsEngine; }

namespace NBody::Core {

    class Application final : public Core::MessageSubscriber<Application> {
    public:
        friend class Core::MessageSubscriber<Application>;

        using Result = std::expected<void, ApplicationError>;

        explicit Application() noexcept;
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) noexcept = delete;
        Application& operator=(Application&&) noexcept = delete;

        [[nodiscard]] Result InitializeSubsystems();
        [[nodiscard]] Result ExecuteMasterLoop();
        void RequestShutdown() noexcept;

    private:
        bool m_isRunning{ false };
        std::atomic<ApplicationState> m_currentState{ ApplicationState::Uninitialized };
        void TransitionToState(ApplicationState requestedState) noexcept;

        std::unique_ptr<Core::MessageBus> m_messageBusHost{};
        void HandleMessage(const SystemMessage& message) noexcept;
        template <typename MessageType>
        void SubscribeToMessage() {
            m_messageBusHost->template Subscribe<MessageType>([this](const Core::SystemMessage& message) {
                HandleMessage(message);
            });
        }

        std::shared_ptr<Particle::ParticleBuffer>   m_sharedParticleBuffer{};
        std::unique_ptr<Generator::GeneratorEngine> m_generatorEngineHost{};
        std::unique_ptr<Render::RenderingEngine>    m_renderingEngineHost{};
        std::unique_ptr<UI::UIManager>              m_uiManagerHost{};
        std::unique_ptr<Physics::PhysicsEngine>     m_physicsEngineHost{};
    };

} // namespace NBody::Core