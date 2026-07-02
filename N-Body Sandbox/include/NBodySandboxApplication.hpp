#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <Core/Events.hpp>

class MessageBus;
class PhysicsEngine;
class RenderingEngine;
class UIManager;
class SimulationStateBuffer;

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

    std::unique_ptr<MessageBus> m_messageBus{};

    std::shared_ptr<SimulationStateBuffer> m_simulationStateBuffer{};

    std::unique_ptr<PhysicsEngine> m_physicsEngine{};
    std::unique_ptr<RenderingEngine> m_renderingEngine{};
    std::unique_ptr<UIManager> m_uiManager{};
    

    std::jthread m_physicsThread;

    void physicsThreadLoop(std::stop_token stopToken);
    void handleMessage(const SystemMessage& message);
};