#include <UI/UIManager.hpp>

#include <iostream>

#include <imgui.h>
#include <variant>

#include <Core/MessageBus.hpp>
#include <UI/Panels/TitleScreenPanel.hpp>
#include <UI/Panels/ModeSelectionPanel.hpp>
#include <UI/Panels/ComputeConfigPanel.hpp>

UIManager::UIManager(MessageBus& messageBus)
    : m_messageBus(messageBus)
{
    messageBus.subscribe<CmdRequestUIStateChange>([this](const SystemMessage& message) {
        handleMessage(message);
    });

    registerPanels();
}

void UIManager::update() {
    auto it = m_statePanels.find(m_UIState);
    if (it == m_statePanels.end()) {
        return;
    }

    for (auto& panel : it->second) {
        panel->draw(m_messageBus);
    }
}

void UIManager::registerPanels() {
    createPanel<TitleScreenPanel>(UIState::TitleScreen);
    createPanel<ModeSelectionPanel>(UIState::ModeSelection);
    
    createPanel<ComputeConfigPanel>(UIState::PrecomputeConfig);
}


void UIManager::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;

        if constexpr (std::is_same_v<T, CmdRequestUIStateChange>) {
            std::cout << "[UI Manager] UI State change requested.\n";
    
            m_UIState = actualMessage.requestedState;
        }

    }, message);
}