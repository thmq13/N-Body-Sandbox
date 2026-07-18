#pragma once

#include <Core/Message.hpp>

namespace NBody::Core { class MessageBus; }
namespace NBody::Particle { class ParticleBuffer; }
namespace NBody::Generator { class GeneratorEngine; }
namespace NBody::Physics { class PhysicsEngine; }
namespace NBody::Render { class RenderingEngine; }
namespace NBody::UI { class UIManager; }

namespace NBody {
    class NBodySandboxApplication {
    public:
        NBodySandboxApplication();

        ~NBodySandboxApplication();

        NBodySandboxApplication(const NBodySandboxApplication&) = delete;
        NBodySandboxApplication& operator=(const NBodySandboxApplication&) = delete;
        NBodySandboxApplication(NBodySandboxApplication&&) = delete;
        NBodySandboxApplication& operator=(NBodySandboxApplication&&) = delete;

        void executeMasterLoop();
    private:
        bool m_isRunning{ true };
        Core::AppState m_appState{ Core::AppState::TitleScreen };

        std::unique_ptr<Core::MessageBus> m_messageBus{};
        std::shared_ptr<Particle::ParticleBuffer> m_particleBuffer{};
        std::unique_ptr<Generator::GeneratorEngine> m_generatorEngine{};
        std::unique_ptr<Physics::PhysicsEngine> m_physicsEngine{};
        std::unique_ptr<Render::RenderingEngine> m_renderingEngine{};
        std::unique_ptr<UI::UIManager> m_uiManager{};

        void handleMessage(const Core::SystemMessage& message);
    };
}
