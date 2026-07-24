#include <UI/UIManager.hpp>

#include <cstddef>
#include <iostream>
#include <variant>

#include <Core/ApplicationState.hpp>
#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <type_traits>
#include <UI/Panels/BackButtonPanel.hpp>
#include <UI/Panels/ModeSelectionPanel.hpp>
#include <UI/Panels/GeneratorPanel.hpp>
#include <UI/Panels/PhysicsPanel.hpp>
#include <UI/Panels/TitleScreenPanel.hpp>

namespace NBody::UI {

    UIManager::UIManager(Core::MessageBus& messageBus) : m_messageBus(messageBus) 
    {
        m_messageBus.subscribe<Core::CmdSendSchemas>([this](const Core::SystemMessage& message) {
            handleMessage(message);
        });

        m_storage.resize(Core::moduleCount);
        for (std::size_t moduleIdx = 0; moduleIdx < Core::moduleCount; ++moduleIdx) {
            m_storage[moduleIdx].resize(Core::subModuleCount[moduleIdx]);
        }

        registerPanels();

        std::cout << "[UI Manager] I need all schemas.\n";
        m_messageBus.publish(Core::CmdRequestSchemas{});
    }

    void UIManager::draw(Core::ApplicationState state) {
        auto it = m_statePanels.find(state);
        if (it == m_statePanels.end()) {
            return;
        }

        for (auto& panel : it->second) {
            panel->draw(m_messageBus);
        }
    }

    void UIManager::registerPanels() {
        createPanel<TitleScreenPanel>(Core::ApplicationState::TitleScreen);

        createPanel<ModeSelectionPanel>(Core::ApplicationState::ModeSelection);
        createPanel<BackButtonPanel>(Core::ApplicationState::ModeSelection, Core::ApplicationState::TitleScreen);

        createPanel<GeneratorPanel>(Core::ApplicationState::PrecomputeConfig, m_storage);
        createPanel<PhysicsPanel>(Core::ApplicationState::PrecomputeConfig, m_storage);
        createPanel<BackButtonPanel>(Core::ApplicationState::PrecomputeConfig, Core::ApplicationState::ModeSelection);

        createPanel<BackButtonPanel>(Core::ApplicationState::RealTimeConfig, Core::ApplicationState::ModeSelection);

        createPanel<BackButtonPanel>(Core::ApplicationState::PlaybackConfig, Core::ApplicationState::ModeSelection);
    }

    void UIManager::handleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& msg) {
            using T = std::decay_t<decltype(msg)>;
            if constexpr (std::is_same_v<T, Core::CmdSendSchemas>) {
                std::size_t moduleIdx = static_cast<std::size_t>(msg.moduleId);
                std::size_t subModuleIdx = static_cast<std::size_t>(msg.subModuleId);
                m_storage[moduleIdx][subModuleIdx][msg.senderId] = msg.schemas;
            }
        }, message);
    }
}