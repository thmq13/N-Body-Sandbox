#pragma once

#include <raylib.h>

class RenderingEngine {
public:
    RenderingEngine();

    ~RenderingEngine();

    RenderingEngine(const RenderingEngine&) = delete;
    RenderingEngine& operator=(const RenderingEngine&) = delete;
    RenderingEngine(RenderingEngine&&) = delete;
    RenderingEngine& operator=(RenderingEngine&&) = delete;

    void pollEvents();

    void beginFrame();
    void endFrame();

    [[nodiscard]] bool shouldWindowClose() const noexcept { return WindowShouldClose(); }
private:
    Camera3D m_simulationCamera{};
};