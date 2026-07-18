#pragma once

#include <raylib.h>

namespace NBody::Render {
    class RenderingEngine {
    public:
        RenderingEngine();
        ~RenderingEngine();

        RenderingEngine(const RenderingEngine&) = delete;
        RenderingEngine& operator=(const RenderingEngine&) = delete;
        RenderingEngine(RenderingEngine&&) = delete;
        RenderingEngine& operator=(RenderingEngine&&) = delete;

        void beginFrame();
        void endFrame();
    private:
        Camera3D m_simulationCamera{};
    };
}
