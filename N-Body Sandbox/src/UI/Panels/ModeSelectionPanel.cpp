#include <UI/Panels/ModeSelectionPanel.hpp>

#include <imgui.h>

#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <Core/ApplicationState.hpp>

namespace NBody::UI {

    namespace {
        namespace ModeSelectionConfig {
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
            constexpr float defaultFontScale{ 1.0f };

            constexpr ImVec4 textColor{ 0.7f, 0.7f, 0.7f, 1.0f };
            constexpr ImVec4 dimTextColor{ 0.45f, 0.45f, 0.45f, 1.0f };
            constexpr ImVec4 buttonBackgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };
            constexpr ImVec4 buttonHoveredColor{ 0.2f, 0.2f, 0.2f, 0.6f };
            constexpr ImVec4 buttonActiveColor{ 0.4f, 0.4f, 0.4f, 0.8f };
            constexpr int    styleColorCount{ 4 };

            constexpr float       headerTextScale{ 5.0f };
            constexpr float       headerHeightScale{ 0.12f };
            constexpr const char* headerText{ "S E L E C T   M O D E" };

            constexpr float buttonTextScale{ 3.5f };
            constexpr float descTextScale{ 1.7f };

            constexpr float       realTimeButtonHeightScale{ 0.30f };
            constexpr const char* realTimeButtonText{ "Real-Time" };
            constexpr float       realTimeDescHeightScale{ 0.37f };
            constexpr const char* realTimeDescText{ "Unfold the moment, frame by frame" };

            constexpr float       precomputeButtonHeightScale{ 0.46f };
            constexpr const char* precomputeButtonText{ "Precompute" };
            constexpr float       precomputeDescHeightScale{ 0.53f };
            constexpr const char* precomputeDescText{ "Let it all happen" };

            constexpr float       playbackButtonHeightScale{ 0.62f };
            constexpr const char* playbackButtonText{ "Playback" };
            constexpr float       playbackDescHeightScale{ 0.69f };
            constexpr const char* playbackDescText{ "Echoes of a time" };
        }
    }

    void ModeSelectionPanel::Draw(Core::MessageBus& messageBus) {
        SetUpWindowAndStyle();

        const ImVec2 windowSize = ImGui::GetWindowSize();

        DrawHeaderText(windowSize);
        DrawRealTimeOption(windowSize, messageBus);
        DrawPrecomputeOption(windowSize, messageBus);
        DrawPlaybackOption(windowSize, messageBus);

        CleanUp();
    }

    void ModeSelectionPanel::SetUpWindowAndStyle() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("ModeSelection", nullptr, ModeSelectionConfig::windowFlags);
        ImGui::PopStyleVar();

        ImGui::PushStyleColor(ImGuiCol_Text, ModeSelectionConfig::textColor);
        ImGui::PushStyleColor(ImGuiCol_Button, ModeSelectionConfig::buttonBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ModeSelectionConfig::buttonHoveredColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ModeSelectionConfig::buttonActiveColor);
    }


    void ModeSelectionPanel::CleanUp() {
        ImGui::SetWindowFontScale(ModeSelectionConfig::defaultFontScale);
        ImGui::PopStyleColor(ModeSelectionConfig::styleColorCount);
        ImGui::End();
    }

    void ModeSelectionPanel::DrawHeaderText(const ImVec2& windowSize) {
        ImGui::SetWindowFontScale(ModeSelectionConfig::headerTextScale);

        const ImVec2 headerSize = ImGui::CalcTextSize(ModeSelectionConfig::headerText);
        const float headerX = (windowSize.x - headerSize.x) * ModeSelectionConfig::centerScaleX;
        const float headerY = windowSize.y * ModeSelectionConfig::headerHeightScale;
        ImGui::SetCursorPos(ImVec2{ headerX, headerY });
        ImGui::TextUnformatted(ModeSelectionConfig::headerText);

        ImGui::SetWindowFontScale(ModeSelectionConfig::defaultFontScale);
    }

    void ModeSelectionPanel::DrawRealTimeOption(const ImVec2& windowSize, Core::MessageBus& messageBus) {
        ImGui::SetWindowFontScale(ModeSelectionConfig::buttonTextScale);

        const ImVec2 btnSize = ImGui::CalcTextSize(ModeSelectionConfig::realTimeButtonText);
        const float btnX = (windowSize.x - btnSize.x) * ModeSelectionConfig::centerScaleX;
        const float btnY = windowSize.y * ModeSelectionConfig::realTimeButtonHeightScale;
        ImGui::SetCursorPos(ImVec2{ btnX, btnY });
        if (ImGui::Button(ModeSelectionConfig::realTimeButtonText)) {
            messageBus.Publish(Core::CmdRequestStateChange{ Core::ApplicationState::RealTimeConfig });
        }

        ImGui::SetWindowFontScale(ModeSelectionConfig::descTextScale);

        const ImVec2 descSize = ImGui::CalcTextSize(ModeSelectionConfig::realTimeDescText);
        const float descX = (windowSize.x - descSize.x) * ModeSelectionConfig::centerScaleX;
        const float descY = windowSize.y * ModeSelectionConfig::realTimeDescHeightScale;
        ImGui::SetCursorPos(ImVec2{ descX, descY });

        ImGui::PushStyleColor(ImGuiCol_Text, ModeSelectionConfig::dimTextColor);
        ImGui::TextUnformatted(ModeSelectionConfig::realTimeDescText);
        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(ModeSelectionConfig::defaultFontScale);
    }

    void ModeSelectionPanel::DrawPrecomputeOption(const ImVec2& windowSize, Core::MessageBus& messageBus) {
        ImGui::SetWindowFontScale(ModeSelectionConfig::buttonTextScale);

        const ImVec2 btnSize = ImGui::CalcTextSize(ModeSelectionConfig::precomputeButtonText);
        const float btnX = (windowSize.x - btnSize.x) * ModeSelectionConfig::centerScaleX;
        const float btnY = windowSize.y * ModeSelectionConfig::precomputeButtonHeightScale;
        ImGui::SetCursorPos(ImVec2{ btnX, btnY });
        if (ImGui::Button(ModeSelectionConfig::precomputeButtonText)) {
            messageBus.Publish(Core::CmdRequestStateChange{ Core::ApplicationState::PrecomputeConfig });
        }

        ImGui::SetWindowFontScale(ModeSelectionConfig::descTextScale);

        const ImVec2 descSize = ImGui::CalcTextSize(ModeSelectionConfig::precomputeDescText);
        const float descX = (windowSize.x - descSize.x) * ModeSelectionConfig::centerScaleX;
        const float descY = windowSize.y * ModeSelectionConfig::precomputeDescHeightScale;
        ImGui::SetCursorPos(ImVec2{ descX, descY });

        ImGui::PushStyleColor(ImGuiCol_Text, ModeSelectionConfig::dimTextColor);
        ImGui::TextUnformatted(ModeSelectionConfig::precomputeDescText);
        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(ModeSelectionConfig::defaultFontScale);
    }

    void ModeSelectionPanel::DrawPlaybackOption(const ImVec2& windowSize, Core::MessageBus& messageBus) {
        ImGui::SetWindowFontScale(ModeSelectionConfig::buttonTextScale);

        const ImVec2 btnSize = ImGui::CalcTextSize(ModeSelectionConfig::playbackButtonText);
        const float btnX = (windowSize.x - btnSize.x) * ModeSelectionConfig::centerScaleX;
        const float btnY = windowSize.y * ModeSelectionConfig::playbackButtonHeightScale;
        ImGui::SetCursorPos(ImVec2{ btnX, btnY });
        if (ImGui::Button(ModeSelectionConfig::playbackButtonText)) {
            messageBus.Publish(Core::CmdRequestStateChange{ Core::ApplicationState::PlaybackConfig });
        }

        ImGui::SetWindowFontScale(ModeSelectionConfig::descTextScale);

        const ImVec2 descSize = ImGui::CalcTextSize(ModeSelectionConfig::playbackDescText);
        const float descX = (windowSize.x - descSize.x) * ModeSelectionConfig::centerScaleX;
        const float descY = windowSize.y * ModeSelectionConfig::playbackDescHeightScale;
        ImGui::SetCursorPos(ImVec2{ descX, descY });

        ImGui::PushStyleColor(ImGuiCol_Text, ModeSelectionConfig::dimTextColor);
        ImGui::TextUnformatted(ModeSelectionConfig::playbackDescText);
        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(ModeSelectionConfig::defaultFontScale);
    }
}