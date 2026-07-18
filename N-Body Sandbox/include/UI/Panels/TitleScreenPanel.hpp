#pragma once

#include <UI/IPanel.hpp>

#include <imgui.h>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class TitleScreenPanel : public IPanel {
    public:
        ~TitleScreenPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;

    private:
        void setUpWindowAndStyle();
        void cleanUp();

        void drawDec(const ImVec2& windowSize);
        void drawTitleText(const ImVec2& windowSize);
        void drawStartButton(const ImVec2& windowSize, Core::MessageBus& messageBus);
        void drawExitButton(const ImVec2& windowSize, Core::MessageBus& messageBus);
    };
}
