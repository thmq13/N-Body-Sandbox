#include <NBodySandboxApplication.hpp>

#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Generator/GeneratorEngine.hpp>
#include <Particle/ParticleBuffer.hpp>
#include <Physics/PhysicsEngine.hpp>
#include <Render/RenderingEngine.hpp>
#include <UI/UIManager.hpp>

namespace NBody {
    NBodySandboxApplication::NBodySandboxApplication()
        : m_messageBus(std::make_unique<Core::MessageBus>()),
        m_particleBuffer(std::make_shared<Particle::ParticleBuffer>()),
        m_generatorEngine(std::make_unique<Generator::GeneratorEngine>(*m_messageBus)),
        m_renderingEngine(std::make_unique<Render::RenderingEngine>(*m_messageBus, m_particleBuffer)),
        m_uiManager(std::make_unique<UI::UIManager>(*m_messageBus)),
        m_physicsEngine(std::make_unique<Physics::PhysicsEngine>(*m_messageBus, m_particleBuffer))
    {
        m_messageBus->subscribe<Core::CmdExitApplication>([this](const Core::SystemMessage& message) {
            handleMessage(message);
        });

        m_messageBus->subscribe<Core::CmdRequestStateChange>([this](const Core::SystemMessage& message) {
            handleMessage(message);
        });

        std::cout << "[App Core] Core engines allocated and threads dispatched.\n";
    }

    NBodySandboxApplication::~NBodySandboxApplication() {
        std::cout << "[App Core] Complete. All resources freed cleanly.\n";
    }

    void NBodySandboxApplication::executeMasterLoop() {
        std::cout << "[App Core] Entering master execution loop.\n";
        while (m_isRunning) {
            m_messageBus->dispatch();

            m_renderingEngine->beginFrame();

            m_renderingEngine->render(m_appState);
            m_uiManager->draw(m_appState);

            m_renderingEngine->endFrame();
        }
    }

    void NBodySandboxApplication::handleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& msg) {
            using T = std::decay_t<decltype(msg)>;

            if constexpr (std::is_same_v<T, Core::CmdExitApplication>) {
                std::cout << "[App Core] Exit command acknowledged. Initiating shutdown sequence...\n";
                m_isRunning = false;
            }

            else if constexpr (std::is_same_v<T, Core::CmdRequestStateChange>) {
                std::cout << "[App Core] Intercepted state change request.\n";
                m_appState = msg.requestedState;
            }

        }, message);
    }
}


