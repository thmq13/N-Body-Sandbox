#pragma once

#include <UI/IPanel.hpp>

#include <imgui.h>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {

    class TitleScreenPanel final : public IPanel {
    public:
        ~TitleScreenPanel() override = default;

        void Draw(Core::MessageBus& messageBus) override;

    private:
        void SetUpWindowAndStyle();
        void CleanUp();

        void DrawDec(const ImVec2& windowSize);
        void DrawTitleText(const ImVec2& windowSize);
        void DrawStartButton(const ImVec2& windowSize, Core::MessageBus& messageBus);
        void DrawExitButton(const ImVec2& windowSize, Core::MessageBus& messageBus);
    };

} // namespace NBody::UI
