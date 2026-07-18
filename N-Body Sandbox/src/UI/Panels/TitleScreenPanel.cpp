#include <UI/Panels/TitleScreenPanel.hpp>

#include <cmath>
#include <imgui.h>

#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Core/AppState.hpp>

namespace NBody::UI {

    namespace {
        namespace TitleScreenConfig {
            constexpr ImGuiWindowFlags windowFlags{
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus
            };

            constexpr float centerScaleX{ 0.5f };
            constexpr float centerScaleY{ 0.45f };

            constexpr float circleDecScaleRadius{ 0.35f };
            constexpr ImU32 circleDecColor{ IM_COL32(50, 50, 50, 255) };
            constexpr int   circleDecSegments{ 64 };
            constexpr float circleDecThickness{ 3.0f };

            constexpr float bodyDecRadius{ 3.0f };
            constexpr float bodyDecSpeed{ 0.5f };
            constexpr ImU32 bodyDecColor{ IM_COL32(180, 180, 180, 150) };

            constexpr ImVec4 textColor{ 0.7f, 0.7f, 0.7f, 1.0f };
            constexpr ImVec4 buttonBackgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };
            constexpr ImVec4 buttonHoveredColor{ 0.2f, 0.2f, 0.2f, 0.6f };
            constexpr ImVec4 buttonActiveColor{ 0.4f, 0.4f, 0.4f, 0.8f };
            constexpr int    styleColorCount{ 4 };

            constexpr float titleTextScale{ 7.50f };
            constexpr float titleHeightScale{ 0.35f };
            constexpr const char* titleText{ "N - B O D Y" };

            constexpr float buttonTextScale{ 4.0f };
            constexpr float startButtonHeightScale{ 0.60f };
            constexpr float exitButtonHeightScale{ 0.70f };
            constexpr const char* startButtonText{ "> start <" };
            constexpr const char* exitButtonText{ "exit" };

            constexpr float defaultFontScale{ 1.0f };
        }
    }

    void TitleScreenPanel::draw(Core::MessageBus& messageBus) {
        setUpWindowAndStyle();

        const ImVec2 windowSize = ImGui::GetWindowSize();

        drawDec(windowSize);
        drawTitleText(windowSize);
        drawStartButton(windowSize, messageBus);
        drawExitButton(windowSize, messageBus);

        cleanUp();
    }

    void TitleScreenPanel::setUpWindowAndStyle() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("TitleScreen", nullptr, TitleScreenConfig::windowFlags);
        ImGui::PopStyleVar();

        ImGui::PushStyleColor(ImGuiCol_Text, TitleScreenConfig::textColor);
        ImGui::PushStyleColor(ImGuiCol_Button, TitleScreenConfig::buttonBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TitleScreenConfig::buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, TitleScreenConfig::buttonActiveColor);
    }

    void TitleScreenPanel::cleanUp() {
        ImGui::SetWindowFontScale(TitleScreenConfig::defaultFontScale);
        ImGui::PopStyleColor(TitleScreenConfig::styleColorCount);
        ImGui::End();
    }

    void TitleScreenPanel::drawDec(const ImVec2& windowSize) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 center = ImVec2(
            windowSize.x * TitleScreenConfig::centerScaleX,
            windowSize.y * TitleScreenConfig::centerScaleY
        );

        const float orbitRadius = windowSize.y * TitleScreenConfig::circleDecScaleRadius;
        drawList->AddCircle(
            center,
            orbitRadius,
            TitleScreenConfig::circleDecColor,
            TitleScreenConfig::circleDecSegments,
            TitleScreenConfig::circleDecThickness
        );

        const float time = static_cast<float>(ImGui::GetTime());
        const float angle = time * TitleScreenConfig::bodyDecSpeed;
        const ImVec2 bodyPos = ImVec2(
            center.x + cosf(angle) * orbitRadius,
            center.y + sinf(angle) * orbitRadius
        );
        drawList->AddCircleFilled(
            bodyPos,
            TitleScreenConfig::bodyDecRadius,
            TitleScreenConfig::bodyDecColor
        );
    }

    void TitleScreenPanel::drawTitleText(const ImVec2& windowSize) {
        ImGui::SetWindowFontScale(TitleScreenConfig::titleTextScale);

        const ImVec2 titleSize = ImGui::CalcTextSize(TitleScreenConfig::titleText);
        ImGui::SetCursorPos(
            ImVec2{
            (windowSize.x - titleSize.x) * TitleScreenConfig::centerScaleX,
             windowSize.y * TitleScreenConfig::titleHeightScale }
             );

        ImGui::TextUnformatted(TitleScreenConfig::titleText);

        ImGui::SetWindowFontScale(TitleScreenConfig::defaultFontScale);
    }

    void TitleScreenPanel::drawStartButton(const ImVec2& windowSize, Core::MessageBus& messageBus) {
        ImGui::SetWindowFontScale(TitleScreenConfig::buttonTextScale);

        const ImVec2 startSize = ImGui::CalcTextSize(TitleScreenConfig::startButtonText);

        ImGui::SetCursorPos(
            ImVec2{
                (windowSize.x - startSize.x) * TitleScreenConfig::centerScaleX,
                 windowSize.y * TitleScreenConfig::startButtonHeightScale }
                 );

        if (ImGui::Button(TitleScreenConfig::startButtonText)) {
            messageBus.publish(Core::CmdRequestStateChange{ Core::AppState::ModeSelection });
        }

        ImGui::SetWindowFontScale(TitleScreenConfig::defaultFontScale);
    }

    void TitleScreenPanel::drawExitButton(const ImVec2& windowSize, Core::MessageBus& messageBus) {
        ImGui::SetWindowFontScale(TitleScreenConfig::buttonTextScale);

        const ImVec2 exitSize = ImGui::CalcTextSize(TitleScreenConfig::exitButtonText);

        ImGui::SetCursorPos(
            ImVec2{
                (windowSize.x - exitSize.x) * TitleScreenConfig::centerScaleX,
                 windowSize.y * TitleScreenConfig::exitButtonHeightScale }
        );

        if (ImGui::Button(TitleScreenConfig::exitButtonText)) {
            messageBus.publish(Core::CmdExitApplication{});
        }

        ImGui::SetWindowFontScale(TitleScreenConfig::defaultFontScale);
    }
}
