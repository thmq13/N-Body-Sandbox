#pragma once

#include <cstdint>
#include <memory>

#include <raylib.h>

#include <Core/Message.hpp>
#include <Core/AppState.hpp>

namespace NBody::Core { class MessageBus; }
namespace NBody::Particle { class ParticleBuffer; }

namespace NBody::Render {
    class InstancedMeshRenderer;

    class RenderingEngine {
    public:
        struct Parameters {
            std::uint32_t targetFPS{ 60 };
        };

        RenderingEngine(Core::MessageBus& messageBus, std::shared_ptr<Particle::ParticleBuffer> particleBuffer);
        ~RenderingEngine();

        RenderingEngine(const RenderingEngine&) = delete;
        RenderingEngine& operator=(const RenderingEngine&) = delete;
        RenderingEngine(RenderingEngine&&) = delete;
        RenderingEngine& operator=(RenderingEngine&&) = delete;
        
        void render(Core::AppState state);

        void beginFrame();
        void endFrame();
    private:
        Core::MessageBus& m_messageBus;
        Parameters m_parameters{};
        std::shared_ptr<Particle::ParticleBuffer> m_particleBuffer{};
        std::unique_ptr<InstancedMeshRenderer> m_instancedMeshRenderer{};
        Camera3D m_camera{};

        void handleMessage(const Core::SystemMessage& message);
        void updateCamera();
        bool shouldDrawParticles(Core::AppState state);


    };
}
