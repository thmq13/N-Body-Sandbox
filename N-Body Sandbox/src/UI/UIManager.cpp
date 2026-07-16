#include <UI/UIManager.hpp>

#include <iostream>
#include <variant>
#include <cstddef>

#include <imgui.h>

#include <Core/MessageBus.hpp>
#include <UI/IPanel.hpp>
#include <UI/Panels/BackButtonPanel.hpp>
#include <UI/Panels/TitleScreenPanel.hpp>
#include <UI/Panels/ModeSelectionPanel.hpp>
#include <UI/Panels/SimulationConfigPanel.hpp>

UIManager::UIManager(MessageBus& messageBus)
    : m_messageBus(messageBus)
{
    m_messageBus.subscribe<CmdSendSchemas>([this](const SystemMessage& message) {
        handleMessage(message);
    });
    std::cout << "[UI Manager] I need all schemas\n";
    m_messageBus.publish(CmdRequestSchemas{});

    for (std::size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx) {
        m_storage[moduleIdx].resize(subModuleCount[moduleIdx]);
    }

    registerPanels();
}

void UIManager::draw(AppState state) {
    auto it = m_statePanels.find(state);
    if (it == m_statePanels.end()) {
        return;
    }

    for (auto& panel : it->second) {
        panel->draw(m_messageBus);
    }
}

void UIManager::registerPanels() {
    createPanel<TitleScreenPanel>(AppState::TitleScreen);

    createPanel<ModeSelectionPanel>(AppState::ModeSelection);
    createPanel<BackButtonPanel>(AppState::ModeSelection, AppState::TitleScreen);
    
    createPanel<SimulationConfigPanel>(AppState::PrecomputeConfig, AppState::PrecomputeRunning, m_storage);
    createPanel<BackButtonPanel>(AppState::PrecomputeConfig, AppState::ModeSelection);


    createPanel<BackButtonPanel>(AppState::RealTimeConfig, AppState::ModeSelection);


    createPanel<BackButtonPanel>(AppState::PlaybackConfig, AppState::ModeSelection);
}


void UIManager::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;
        if constexpr (std::is_same_v<T, CmdSendSchemas>) {
            std::size_t moduleIdx = static_cast<std::size_t>(actualMessage.moduleId);
            std::size_t subModuleIdx = static_cast<std::size_t>(actualMessage.subModuleId);
            m_storage[moduleIdx][subModuleIdx][actualMessage.senderId] = actualMessage.schemas;
        }    
    }, message);
}