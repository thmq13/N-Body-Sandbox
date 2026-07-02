#include <UI/UIManager.hpp>

#include <iostream>

#include <imgui.h>
#include <variant>

#include <Core/MessageBus.hpp>
#include <UI/Panels/TitleScreenPanel.hpp>
#include <UI/Panels/ModeSelectionPanel.hpp>
#include <UI/Panels/ComputeConfigPanel.hpp>
#include <UI/Panels/PhysicsConfigPanel.hpp>
#include <UI/Panels/SpawnerConfigPanel.hpp>

UIManager::UIManager(MessageBus& messageBus)
    : m_messageBus(messageBus)
{
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
    
    createPanel<ComputeConfigPanel>(AppState::PrecomputeConfig);
    createPanel<PhysicsConfigPanel>(AppState::PrecomputeConfig);
    createPanel<SpawnerConfigPanel>(AppState::PrecomputeConfig);
}


void UIManager::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;

    }, message);
}