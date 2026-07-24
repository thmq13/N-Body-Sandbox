#pragma once

#include <UI/IPanel.hpp>

#include <imgui.h>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {

    class ModeSelectionPanel final  : public IPanel {
    public:
        ~ModeSelectionPanel() override = default;

        void Draw(Core::MessageBus& messageBus) override;

    private:
        void SetUpWindowAndStyle();
        void CleanUp();

        void DrawHeaderText(const ImVec2& windowSize);
        void DrawRealTimeOption(const ImVec2& windowSize, Core::MessageBus& messageBus);
        void DrawPrecomputeOption(const ImVec2& windowSize, Core::MessageBus& messageBus);
        void DrawPlaybackOption(const ImVec2& windowSize, Core::MessageBus& messageBus);
    };

} // namespace NBody::UI
