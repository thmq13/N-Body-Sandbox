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
        m_messageBus.Subscribe<Core::CmdSendSchemas>([this](const Core::SystemMessage& message) {
            HandleMessage(message);
        });

        m_storage.resize(Core::moduleCount);
        for (std::size_t moduleIdx = 0; moduleIdx < Core::moduleCount; ++moduleIdx) {
            m_storage[moduleIdx].resize(Core::subModuleCount[moduleIdx]);
        }

        RegisterPanels();

        std::cout << "[UI Manager] I need all schemas.\n";
        m_messageBus.Publish(Core::CmdRequestSchemas{});
    }

    void UIManager::Draw(Core::ApplicationState state) {
        auto it = m_statePanels.find(state);
        if (it == m_statePanels.end()) {
            return;
        }

        for (auto& panel : it->second) {
            panel->Draw(m_messageBus);
        }
    }

    void UIManager::RegisterPanels() {
        CreatePanel<TitleScreenPanel>(Core::ApplicationState::TitleScreen);

        CreatePanel<ModeSelectionPanel>(Core::ApplicationState::ModeSelection);
        CreatePanel<BackButtonPanel>(Core::ApplicationState::ModeSelection, Core::ApplicationState::TitleScreen);

        CreatePanel<GeneratorPanel>(Core::ApplicationState::PrecomputeConfig, m_storage);
        CreatePanel<PhysicsPanel>(Core::ApplicationState::PrecomputeConfig, m_storage);
        CreatePanel<BackButtonPanel>(Core::ApplicationState::PrecomputeConfig, Core::ApplicationState::ModeSelection);

        CreatePanel<BackButtonPanel>(Core::ApplicationState::RealTimeConfig, Core::ApplicationState::ModeSelection);

        CreatePanel<BackButtonPanel>(Core::ApplicationState::PlaybackConfig, Core::ApplicationState::ModeSelection);
    }

    void UIManager::HandleMessage(const Core::SystemMessage& message) {
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