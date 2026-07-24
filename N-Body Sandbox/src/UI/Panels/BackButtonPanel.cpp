#include "UI/Panels/BackButtonPanel.hpp"

#include <imgui.h>

#include <Core/ApplicationState.hpp>
#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>

namespace NBody::UI {

    namespace {
        namespace BackButtonConfig {
            constexpr ImGuiWindowFlags windowFlags{
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_AlwaysAutoResize
            };

            constexpr float defaultFontScale{ 1.0f };
            constexpr float buttonTextScale{ 2.5f };

            constexpr ImVec4 textColor{ 1.0f, 1.0f, 1.0f, 1.0f };
            constexpr ImVec4 buttonBackgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };
            constexpr ImVec4 buttonHoveredColor{ 0.2f, 0.2f, 0.2f, 0.6f };
            constexpr ImVec4 buttonActiveColor{ 0.4f, 0.4f, 0.4f, 0.8f };
            constexpr int    styleColorCount{ 4 };

            constexpr const char* buttonText{ "> back" };
            constexpr const char* windowId{ "BackButtonWindow" };

            constexpr float offsetX{ 30.0f };
            constexpr float offsetY{ 30.0f };
        } // namespace BackButtonConfig
    }

    BackButtonPanel::BackButtonPanel(Core::ApplicationState nextState) : m_nextState(nextState) {}

    void BackButtonPanel::Draw(Core::MessageBus& messageBus) {

        SetUpWindowAndStyle();

        ImGui::SetWindowFontScale(BackButtonConfig::buttonTextScale);

        if (ImGui::Button(BackButtonConfig::buttonText)) {
            messageBus.Publish(Core::CmdRequestStateChange{ m_nextState });
        }

        CleanUp();
    }

    void BackButtonPanel::SetUpWindowAndStyle() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 windowPos(
            viewport->WorkPos.x + BackButtonConfig::offsetX,
            viewport->WorkPos.y + viewport->WorkSize.y - BackButtonConfig::offsetY
        );

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(0.0f, 1.0f));

        ImGui::Begin(BackButtonConfig::windowId, nullptr, BackButtonConfig::windowFlags);

        ImGui::PushStyleColor(ImGuiCol_Text, BackButtonConfig::textColor);
        ImGui::PushStyleColor(ImGuiCol_Button, BackButtonConfig::buttonBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BackButtonConfig::buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, BackButtonConfig::buttonActiveColor);
    }

    void BackButtonPanel::CleanUp() {
        ImGui::PopStyleColor(BackButtonConfig::styleColorCount);
        ImGui::SetWindowFontScale(BackButtonConfig::defaultFontScale);
        ImGui::End();
    }

} // namespace NBody::UI