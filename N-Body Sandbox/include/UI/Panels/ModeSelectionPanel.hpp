#pragma once

#include <UI/IPanel.hpp>

#include <imgui.h>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class ModeSelectionPanel : public IPanel {
    public:
        ~ModeSelectionPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;

    private:
        void setUpWindowAndStyle();
        void cleanUp();

        void drawHeaderText(const ImVec2& windowSize);
        void drawRealTimeOption(const ImVec2& windowSize, Core::MessageBus& messageBus);
        void drawPrecomputeOption(const ImVec2& windowSize, Core::MessageBus& messageBus);
        void drawPlaybackOption(const ImVec2& windowSize, Core::MessageBus& messageBus);
    };
}
