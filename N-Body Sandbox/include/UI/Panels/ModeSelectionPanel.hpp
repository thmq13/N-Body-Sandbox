#pragma once

#include <UI/IPanel.hpp>

#include <imgui.h>

class ModeSelectionPanel : public IPanel {
public:
    ~ModeSelectionPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    void setUpWindowAndStyle();
    void cleanUp();

    void drawHeaderText(const ImVec2& windowSize);
    void drawRealTimeOption(const ImVec2& windowSize, MessageBus& messageBus);
    void drawPrecomputeOption(const ImVec2& windowSize, MessageBus& messageBus);
    void drawPlaybackOption(const ImVec2& windowSize, MessageBus& messageBus);
    void drawBackButton(const ImVec2& windowSize, MessageBus& messageBus);
};