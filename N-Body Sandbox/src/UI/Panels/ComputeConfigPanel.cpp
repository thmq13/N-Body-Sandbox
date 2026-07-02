#include <UI/Panels/ComputeConfigPanel.hpp>

#include <imgui.h>


namespace {
    namespace ComputeConfigConfig {
        constexpr ImGuiWindowFlags windowFlags{
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus
        };

        constexpr float widthScale{ 0.30f };
        constexpr float heightScale{ 0.40f };
        constexpr float offsetX{ 60.0f };
        constexpr float offsetY{ 80.0f };
        constexpr float defaultFontScale{ 1.0f };
        constexpr float configFontScale{ 1.5f };
        constexpr float comboWidthScale{ 0.3f };
        constexpr float frameBorderSize{ 1.0f };

        constexpr ImVec4 pureBlack{ 0.0f, 0.0f, 0.0f, 1.0f };
        constexpr ImVec4 mutedGray{ 0.2f, 0.2f, 0.2f, 1.0f };
        constexpr ImVec4 activeGray{ 0.3f, 0.3f, 0.3f, 1.0f };
        constexpr ImVec4 borderWhite{ 1.0f, 1.0f, 1.0f, 0.5f };
        constexpr ImVec4 retroWhite{ 1.0f, 1.0f, 1.0f, 1.0f };
        constexpr int    styleColorCount{ 11 };
        constexpr int    comboStyleColorCount{ 10 };
        constexpr int    checkboxStyleColorCount{ 5 };

        constexpr const char* windowName{ "Compute Configuration" };
        constexpr const char* headerText{ "== COMPUTE ENGINE CONFIGURATION ==" };
        constexpr const char* hwAndAlgoText{ "> HARDWARE AND ALGORITHM" };
        constexpr const char* algoLabel{ "Algorithm" };
        constexpr const char* algoOptions{ "Direct N^2\0Barnes-Hut\0" };
        constexpr const char* hwLabel{ "Hardware Target" };
        constexpr const char* hwOptions{ "CPU\0GPU\0" };
        constexpr const char* optText{ "> OPTIMIZATIONS" };
        constexpr const char* optOpenMP{ "Enable OpenMP Multithreading" };
        constexpr const char* optSIMD{ "Enable SIMD (AVX2/AVX-512)" };
    }
}

void ComputeConfigPanel::draw(MessageBus& messageBus) {
    setUpWindowAndStyle();
    
    drawMainTitle();
    drawCombos();

    ImGui::Spacing();
    ImGui::Spacing();

    drawCheckBoxes();

    cleanUp();
}

void ComputeConfigPanel::setUpWindowAndStyle() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImVec2 calculatedSize(viewport->Size.x * ComputeConfigConfig::widthScale,
        viewport->Size.y * ComputeConfigConfig::heightScale);
    ImVec2 initialPos(viewport->Pos.x + ComputeConfigConfig::offsetX,
        viewport->Pos.y + ComputeConfigConfig::offsetY);

    ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(calculatedSize, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 14.0f));

    ImGui::Begin(ComputeConfigConfig::windowName, nullptr, ComputeConfigConfig::windowFlags);
    ImGui::SetWindowFontScale(ComputeConfigConfig::configFontScale);

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImVec2 currentPos = ImGui::GetWindowPos();
        ImGui::SetWindowPos(ImVec2(currentPos.x + dragDelta.x, currentPos.y + dragDelta.y));
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }
}

void ComputeConfigPanel::cleanUp() {
    ImGui::SetWindowFontScale(ComputeConfigConfig::defaultFontScale);
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void ComputeConfigPanel::drawMainTitle() {
    ImGui::TextUnformatted(ComputeConfigConfig::headerText);
    ImGui::Spacing();
    ImGui::TextUnformatted(ComputeConfigConfig::hwAndAlgoText);
    ImGui::Separator();
    ImGui::Spacing();
}

void ComputeConfigPanel::drawCombos() {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ComputeConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ComputeConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ComputeConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_Border, ComputeConfigConfig::borderWhite);
    ImGui::PushStyleColor(ImGuiCol_Button, ComputeConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ComputeConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ComputeConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_Header, ComputeConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ComputeConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ComputeConfigConfig::borderWhite);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, ComputeConfigConfig::frameBorderSize);

    const float windowSizeX = ImGui::GetWindowWidth();
    const float comboWidth = windowSizeX * ComputeConfigConfig::comboWidthScale;

    int currentAlgo = static_cast<int>(m_localComputeConfig.algorithm);
    ImGui::SetNextItemWidth(comboWidth);
    if (ImGui::Combo(ComputeConfigConfig::algoLabel, &currentAlgo, ComputeConfigConfig::algoOptions)) {
        m_localComputeConfig.algorithm = static_cast<ComputeConfig::Algorithm>(currentAlgo);
    }
    int currentHardware = static_cast<int>(m_localComputeConfig.hardware);
    ImGui::SetNextItemWidth(comboWidth);
    if (ImGui::Combo(ComputeConfigConfig::hwLabel, &currentHardware, ComputeConfigConfig::hwOptions)) {
        m_localComputeConfig.hardware = static_cast<ComputeConfig::Hardware>(currentHardware);
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(ComputeConfigConfig::comboStyleColorCount);
}

void ComputeConfigPanel::drawCheckBoxes() {
    ImGui::TextUnformatted(ComputeConfigConfig::optText);
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ComputeConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ComputeConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ComputeConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_Border, ComputeConfigConfig::borderWhite);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ComputeConfigConfig::retroWhite);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, ComputeConfigConfig::frameBorderSize);

    ImGui::Checkbox(ComputeConfigConfig::optOpenMP, &m_localComputeConfig.useOpenMP);
    ImGui::Spacing();
    ImGui::Checkbox(ComputeConfigConfig::optSIMD, &m_localComputeConfig.useSIMD);

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(ComputeConfigConfig::checkboxStyleColorCount); 
}
