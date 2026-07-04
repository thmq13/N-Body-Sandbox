#pragma once

#include <UI/IPanel.hpp>
#include <AppState.hpp>

#include <imgui.h>

class BackButtonPanel : public IPanel {
public:
    BackButtonPanel(AppState targetRunningState)
        : m_targetRunningState(targetRunningState) {
    }
    ~BackButtonPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    AppState m_targetRunningState;

    void setUpWindowAndStyle();
    void cleanUp();
};