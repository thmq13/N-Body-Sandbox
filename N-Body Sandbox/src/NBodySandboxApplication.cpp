#include <NBodySandboxApplication.hpp>

#include <iostream>
#include <variant>

#include <Core/MessageBus.hpp>
#include <PhysicsEngine/PhysicsEngine.hpp>
#include <RenderingEngine/RenderingEngine.hpp>
#include <SimulationStateBuffer/SimulationStateBuffer.hpp>
#include <UI/UIManager.hpp>

NBodySandboxApplication::NBodySandboxApplication()
    : m_messageBus(std::make_unique<MessageBus>()),

      m_simulationStateBuffer(std::make_unique<SimulationStateBuffer>()),

      m_physicsEngine(std::make_unique<PhysicsEngine>()),
      m_renderingEngine(std::make_unique<RenderingEngine>()),
      m_uiManager(std::make_unique<UIManager>(*m_messageBus)),

      m_physicsThread([this](std::stop_token token) {physicsThreadLoop(token); })
{
    m_messageBus->subscribe<CmdExitApplication>([this](const SystemMessage& message) {
        handleMessage(message);
    }); 

    m_messageBus->subscribe<EvtSimulationError>([this](const SystemMessage& message) {
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

        m_uiManager->update();

        m_renderingEngine->endFrame();
    }
}

void NBodySandboxApplication::physicsThreadLoop(std::stop_token stopToken) {
    std::cout << "[Physics Worker] Dedicated integration pipeline alive.\n";
    
    while (!stopToken.stop_requested()) {


    }

    std::cout << "[Physics Worker] Stop requested. Pipeline safely spun down.\n";
}

void NBodySandboxApplication::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;

        if constexpr (std::is_same_v<T, CmdExitApplication>) {
            std::cout << "[App Core] Exit command acknowledged. Initiating shutdown sequence...\n";

            m_isRunning = false;
            m_physicsThread.request_stop();
        }

        else if constexpr (std::is_same_v<T, EvtSimulationError>) {
            std::cerr << "[App Core] CRITICAL ERROR from module ["
                << actualMessage.moduleName << "]: "
                << actualMessage.errorMessage << "\n";
        }

    }, message);
}

