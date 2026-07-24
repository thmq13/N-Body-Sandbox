#include <Render/RenderingEngine.hpp>

#include <utility>
#include <memory>
#include <iostream>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include <Particle/ParticleBuffer.hpp>
#include <Core/MessageBus.hpp>
#include <Render/InstancedMeshRenderer.hpp>

namespace NBody::Render {
    namespace {
        namespace WindowConfig {
            constexpr int width{ 1900 };
            constexpr int height{ 1050 };
            constexpr const char* title{ "N-Body Sandbox" };
            constexpr unsigned int flags{ FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE };
            constexpr Color clearBackgroundColor{ BLACK };
        };

        namespace CameraConfig {
            constexpr Vector3 position{ 0.0f, 25.0f, 35.0f };
            constexpr Vector3 target{ 0.0f, 0.0f, 0.0f };
            constexpr Vector3 up{ 0.0f, 1.0f, 0.0f };
            constexpr float fovy{ 70.0f };
            constexpr int projection{ CAMERA_PERSPECTIVE };
            constexpr float cameraMoveSpeed{ 15.0f };
            constexpr float cameraPanSpeed{ 5.0f };
        };
    }

    RenderingEngine::RenderingEngine(Core::MessageBus& messageBus, std::shared_ptr<Particle::ParticleBuffer> particleBuffer)
        : m_messageBus(messageBus), 
        m_particleBuffer(std::move(particleBuffer)),
        m_camera(
            CameraConfig::position,
            CameraConfig::target,
            CameraConfig::up,
            CameraConfig::fovy,
            CameraConfig::projection
        )

    {
        SetTraceLogLevel(LOG_NONE);

        SetConfigFlags(WindowConfig::flags);
        InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), WindowConfig::title);
        SetTargetFPS(static_cast<int>(m_parameters.targetFPS));
        
        m_instancedMeshRenderer = std::make_unique<Render::InstancedMeshRenderer>();

        rlImGuiSetup(true);

        std::cout << "[Rendering Engine] Pipeline online.\n";
    }

    RenderingEngine::~RenderingEngine() {
        rlImGuiShutdown();
        CloseWindow();
    }

    void RenderingEngine::render(Core::ApplicationState state) {
        if (!shouldDrawParticles(state)) {
            return;
        }

        m_particleBuffer->updateFrontBuffer();
        const auto& frontBuffer = m_particleBuffer->getFrontBuffer();

        updateCamera();
        BeginMode3D(m_camera);

        DrawGrid(20, 1.0f);
        m_instancedMeshRenderer->draw(frontBuffer);

        EndMode3D();
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

    void RenderingEngine::handleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& msg) {

        }, message);
    }

    void RenderingEngine::updateCamera() {
        static bool lockMovements = true;

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            lockMovements = !lockMovements;
            if (lockMovements) {
                EnableCursor();
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }
            else {
                DisableCursor();
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }
        }

        if (!lockMovements) {
            float deltaTime{ GetFrameTime() };

            Vector3 movement{ 0.0f, 0.0f, 0.0f };
            float currentMoveSpeed{ IsKeyDown(KEY_LEFT_CONTROL)
                ? CameraConfig::cameraMoveSpeed * 2.5f
                : CameraConfig::cameraMoveSpeed };
            if (IsKeyDown(KEY_W)) movement.x += currentMoveSpeed * deltaTime;
            if (IsKeyDown(KEY_S)) movement.x -= currentMoveSpeed * deltaTime;
            if (IsKeyDown(KEY_D)) movement.y += currentMoveSpeed * deltaTime;
            if (IsKeyDown(KEY_A)) movement.y -= currentMoveSpeed * deltaTime;

            if (IsKeyDown(KEY_SPACE)) movement.z += currentMoveSpeed * deltaTime;
            if (IsKeyDown(KEY_LEFT_SHIFT)) movement.z -= currentMoveSpeed * deltaTime;

            Vector3 rotation{ 0.0f, 0.0f, 0.0f };
            Vector2 mouseDelta{ GetMouseDelta() };
            rotation.x = mouseDelta.x * CameraConfig::cameraPanSpeed * deltaTime;
            rotation.y = mouseDelta.y * CameraConfig::cameraPanSpeed * deltaTime;

            UpdateCameraPro(&m_camera, movement, rotation, 0.0f);
        }  
    }

    bool RenderingEngine::shouldDrawParticles(Core::ApplicationState state) {
        return state == Core::ApplicationState::PrecomputeConfig ||
               state == Core::ApplicationState::PrecomputeRunning || 
               state == Core::ApplicationState::RealTimeConfig ||
               state == Core::ApplicationState::RealTimeRunning;
    }
}