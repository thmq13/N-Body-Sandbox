#include <Core/Application.hpp>

#include <Core/Logger.hpp>
#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Generator/GeneratorEngine.hpp>
#include <Particle/ParticleBuffer.hpp>
#include <Physics/PhysicsEngine.hpp>
#include <Render/RenderingEngine.hpp>
#include <UI/UIManager.hpp>

import std;

namespace NBody::Core {

    Application::Application() noexcept = default;

    Application::~Application() {
        if (m_isRunning) {
            RequestShutdown();
        }
        NBODY_INFO("APPLICATION: Deallocating subsystem components.");
    }

    Application::Result Application::InitializeSubsystems() {
        if (m_currentState != ApplicationState::Uninitialized) {
            return std::unexpected(ApplicationError::ErrAlreadyInitialized);
        }

        auto messageBus = std::make_unique<MessageBus>();
        auto sharedParticleBuffer = std::make_shared<Particle::ParticleBuffer>();

        auto generatorEngine = std::make_unique<Generator::GeneratorEngine>(*messageBus);
        auto generatorEngineInitRes{ generatorEngine->InitializeEngine() };
        if (!generatorEngineInitRes) {
            NBODY_FATAL("APPLICATION: Generator engine failed to initialize: {}", generatorEngineInitRes.error());
            return std::unexpected(ApplicationError::ErrGeneratorFailed);
        }

        auto renderingEngine = std::make_unique<Render::RenderingEngine>(*messageBus, sharedParticleBuffer);
        /*auto renderingEngineInitRes{ renderingEngine->InitializeEngine() };
        if (!renderingEngineInitRes) {
            NBODY_FATAL("APPLICATION: Rendering engine failed to initialize: {}", renderingEngineInitRes.error());
            return std::unexpected(ApplicationError::ErrRenderingEngineFailed);
        }*/

        auto uiManager = std::make_unique<UI::UIManager>(*messageBus);
        //auto uiManagerInitRes{ uiManager->InitializeManager() };

        auto physicsEngine = std::make_unique<Physics::PhysicsEngine>(*messageBus, m_currentState, sharedParticleBuffer);
        auto physicsEngineInitRes{ physicsEngine->InitializeEngine() };
        if (!physicsEngineInitRes) {
            NBODY_FATAL("APPLICATION: Physics engine failed to initialize: {}", physicsEngineInitRes.error());
            return std::unexpected(ApplicationError::ErrPhysicsEngineFailed);
        }

        m_messageBusHost = std::move(messageBus);
        m_sharedParticleBuffer = std::move(sharedParticleBuffer);
        m_generatorEngineHost = std::move(generatorEngine);
        m_renderingEngineHost = std::move(renderingEngine);
        m_uiManagerHost = std::move(uiManager);
        m_physicsEngineHost = std::move(physicsEngine);

        NBODY_INFO("APPLICATION: All runtime engines initialized successfully.");

        SubscribeToMessages<CmdExitApplication, CmdRequestStateChange>();
        m_isRunning = true;
        TransitionToState(ApplicationState::TitleScreen);

        return {};
    }

    Application::Result Application::ExecuteMasterLoop() {
        if (!m_isRunning) {
            return std::unexpected(ApplicationError::ErrNotInitialized);
        }

        NBODY_INFO("APPLICATION: Entering master loop.");
        while (m_isRunning) {
            m_messageBusHost->Dispatch();

            m_renderingEngineHost->BeginFrame();
            m_renderingEngineHost->Render(m_currentState);
            m_uiManagerHost->Draw(m_currentState);
            m_renderingEngineHost->EndFrame();
        }

        return {};
    }

    void Application::RequestShutdown() noexcept {
        NBODY_INFO("APPLICATION: Processing exit request...");
        m_isRunning = false;
        TransitionToState(ApplicationState::TearDown);
    }

    void Application::TransitionToState(ApplicationState requestedState) noexcept {
        if (m_currentState.load() == requestedState) {
            return;
        }

        m_currentState.store(requestedState);

        if (m_physicsEngineHost) {
            m_physicsEngineHost->NotifyStateChanged();
        }

        NBODY_INFO("APPLICATION: State changed to: {}.", m_currentState.load());
    }

    void Application::HandleMessage(const SystemMessage& message) noexcept {
        std::visit([this](const auto& msg) noexcept {
            using MessageType = std::decay_t<decltype(msg)>;

            if constexpr (std::is_same_v<MessageType, CmdExitApplication>) {
                NBODY_TRACE("APPLICATION: CmdExitApplication received.");
                RequestShutdown();
            }
            else if constexpr (std::is_same_v<MessageType, CmdRequestStateChange>) {
                NBODY_TRACE("APPLICATION: CmdRequestStateChange received.");
                TransitionToState(msg.requestedState);
            }
        }, message);
    }

    

} // namespace NBody::Core