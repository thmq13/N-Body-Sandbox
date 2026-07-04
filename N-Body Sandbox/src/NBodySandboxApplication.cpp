#include <NBodySandboxApplication.hpp>

#include <iostream>
#include <variant>
#include <chrono>

#include <Core/MessageBus.hpp>
#include <Physics/PhysicsEngine.hpp>
#include <RenderingEngine/RenderingEngine.hpp>
#include <SimulationStateBuffer/SimulationStateBuffer.hpp>
#include <UI/UIManager.hpp>

NBodySandboxApplication::NBodySandboxApplication()
    : m_messageBus(std::make_unique<MessageBus>()),

      m_simulationStateBuffer(std::make_unique<SimulationStateBuffer>()),

      m_physicsEngine(std::make_unique<PhysicsEngine>(*m_messageBus)),
      m_renderingEngine(std::make_unique<RenderingEngine>()),
      m_uiManager(std::make_unique<UIManager>(*m_messageBus))
{
    m_messageBus->subscribe<CmdExitApplication>([this](const SystemMessage& message) {
        handleMessage(message);
    }); 

    m_messageBus->subscribe<CmdRequestStateChange>([this](const SystemMessage& message) {
        handleMessage(message);
    });

    std::cout << "[App Core] Core engines allocated and threads dispatched.\n";
}

NBodySandboxApplication::~NBodySandboxApplication() {
    m_isRunning = false;
    std::cout << "[App Core] Complete. All resources freed cleanly.\n";
}

void NBodySandboxApplication::setUpAndGenerateScene() {
    std::cout << "[App Core] Creating procedural initial conditions.\n";
}

void NBodySandboxApplication::executeMasterLoop() {
    std::cout << "[App Core] Entering master execution loop.\n";
    while (m_isRunning) {
        m_messageBus->dispatch();

        m_renderingEngine->beginFrame();

        m_uiManager->draw(m_appState);

        m_renderingEngine->endFrame();
    }
}

void NBodySandboxApplication::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;

        if constexpr (std::is_same_v<T, CmdExitApplication>) {
            std::cout << "[App Core] Exit command acknowledged. Initiating shutdown sequence...\n";

            m_isRunning = false;
            //...
        }

        else if constexpr (std::is_same_v<T, CmdRequestStateChange>) {
            std::cout << "[App Core] Intercepted state change request.\n";

            m_appState = actualMessage.requestedState;
        }

    }, message);
}

