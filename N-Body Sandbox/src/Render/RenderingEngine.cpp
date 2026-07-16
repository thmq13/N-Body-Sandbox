#include <Render/RenderingEngine.hpp>

#include <iostream>
#include <string_view>

#include <imgui.h>
#include <rlImGui.h>

namespace {
    namespace WindowConfig {
        constexpr int width{ 1900 };
        constexpr int height{ 1050 };
        constexpr std::string_view title{ "N-Body Sandbox" };
        constexpr int targetFPS{ 60 };
        constexpr unsigned int flags{ FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE };
        constexpr Color clearBackgroundColor{ BLACK };
    };

    namespace CameraConfig {
        constexpr Vector3 position{ 0.0f, 25.0f, 35.0f };
        constexpr Vector3 target{ 0.0f, 0.0f, 0.0f };
        constexpr Vector3 up{ 0.0f, 1.0f, 0.0f };
        constexpr float fovy{ 45.0f };
        constexpr int projection{ CAMERA_PERSPECTIVE };
    };
}

RenderingEngine::RenderingEngine() 
{
    SetTraceLogLevel(LOG_NONE);

    SetConfigFlags(WindowConfig::flags);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), WindowConfig::title.data());
    SetTargetFPS(WindowConfig::targetFPS);

    m_simulationCamera.position = CameraConfig::position;
    m_simulationCamera.target = CameraConfig::target;
    m_simulationCamera.up = CameraConfig::up;
    m_simulationCamera.fovy = CameraConfig::fovy;
    m_simulationCamera.projection = CameraConfig::projection;

    rlImGuiSetup(true);

    std::cout << "[Rendering Engine] Pipeline online.\n";
}

RenderingEngine::~RenderingEngine() {
    rlImGuiShutdown();
    CloseWindow();
}

void RenderingEngine::pollEvents()
{
}

void RenderingEngine::beginFrame() {
    BeginDrawing();
    ClearBackground(WindowConfig::clearBackgroundColor);

    rlImGuiBegin();
}

void RenderingEngine::endFrame() {
    rlImGuiEnd();
    EndDrawing();
}
