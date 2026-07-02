#pragma once

#include <UI/IPanel.hpp>

#include <imgui.h>

class TitleScreenPanel : public IPanel {
public:
    ~TitleScreenPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    void setUpWindowAndStyle();
    void cleanUp();

    void drawDec(const ImVec2& windowSize);
    void drawTitleText(const ImVec2& windowSize);
    void drawStartButton(const ImVec2& windowSize, MessageBus& messageBus);
    void drawExitButton(const ImVec2& windowSize, MessageBus& messageBus);
};