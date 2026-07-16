#pragma once

#include <atomic>
#include <memory>

#include <Core/Message.hpp>
#include <Core/AppState.hpp>

class MessageBus;
class GeneratorEngine;
class PhysicsEngine;
class RenderingEngine;
class UIManager;
class ParticleBuffer;

class NBodySandboxApplication {
public:
    NBodySandboxApplication();

    ~NBodySandboxApplication();

    NBodySandboxApplication(const NBodySandboxApplication&) = delete;
    NBodySandboxApplication& operator=(const NBodySandboxApplication&) = delete;
    NBodySandboxApplication(NBodySandboxApplication&&) = delete;
    NBodySandboxApplication& operator=(NBodySandboxApplication&&) = delete;

    void setUpAndGenerateScene();
    void executeMasterLoop();
private:
    std::atomic<bool> m_isRunning{ true };
    std::atomic<AppState> m_appState{ AppState::TitleScreen };

    std::unique_ptr<MessageBus> m_messageBus{};
    std::shared_ptr<ParticleBuffer> m_particleBuffer{};
    std::unique_ptr<GeneratorEngine> m_generatorEngine{};
    std::unique_ptr<PhysicsEngine> m_physicsEngine{};
    std::unique_ptr<RenderingEngine> m_renderingEngine{};
    std::unique_ptr<UIManager> m_uiManager{};

    void handleMessage(const SystemMessage& message);
};