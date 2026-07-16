#pragma once

#include <UI/IPanel.hpp>
#include <Core/AppState.hpp>

#include <imgui.h>

class BackButtonPanel : public IPanel {
public:
    explicit BackButtonPanel(AppState nextState)
        : m_nextState(nextState) {
    }
    ~BackButtonPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    AppState m_nextState;

    void setUpWindowAndStyle();
    void cleanUp();
};