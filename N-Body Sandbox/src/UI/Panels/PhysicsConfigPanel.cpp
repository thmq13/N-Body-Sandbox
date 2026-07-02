#include <UI/Panels/PhysicsConfigPanel.hpp>

#include <imgui.h>

namespace {
    namespace PhysicsConfigConfig {
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
        constexpr float offsetX{ 300.0f };
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
        constexpr int    inputStyleColorCount{ 4 }; 

        constexpr const char* windowName{ "Physics Configuration" };
        constexpr const char* headerText{ "== PHYSICS CONFIGURATION ==" };
        constexpr const char* inteText{ "Select integration method:" };
        constexpr const char* inteLabel{ "Integrator" };
        constexpr const char* inteOptions{ "Euler\0Verlet\0RK4" };
        constexpr const char* constText{ "> Physics constants" };

        constexpr const char* gConstantLabel{ "G Constant" };
        constexpr const char* deltaTimeLabel{ "Delta Time" };
        constexpr const char* softeningLengthLabel{ "Softening Length" };
        constexpr const char* thetaLabel{ "Theta" };

        constexpr const char* highPrecisionFormat{ "%.6f" };
        constexpr const char* medPrecisionFormat{ "%.4f" };
    }
}

void PhysicsConfigPanel::draw(MessageBus& messageBus) {
    setUpWindowAndStyle();

    drawMainTitle();
    drawCombos();

    ImGui::Spacing();
    ImGui::Spacing();

    drawConstantInputs();
    cleanUp();
}

void PhysicsConfigPanel::setUpWindowAndStyle() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImVec2 calculatedSize(viewport->Size.x * PhysicsConfigConfig::widthScale,
        viewport->Size.y * PhysicsConfigConfig::heightScale);
    ImVec2 initialPos(viewport->Pos.x + PhysicsConfigConfig::offsetX,
        viewport->Pos.y + PhysicsConfigConfig::offsetY);

    ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(calculatedSize, ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 14.0f));

    ImGui::Begin(PhysicsConfigConfig::windowName, nullptr, PhysicsConfigConfig::windowFlags);
    ImGui::SetWindowFontScale(PhysicsConfigConfig::configFontScale);

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        ImVec2 currentPos = ImGui::GetWindowPos();
        ImGui::SetWindowPos(ImVec2(currentPos.x + dragDelta.x, currentPos.y + dragDelta.y));
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }
}

void PhysicsConfigPanel::cleanUp() {
    ImGui::SetWindowFontScale(PhysicsConfigConfig::defaultFontScale);
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void PhysicsConfigPanel::drawMainTitle() {
    ImGui::TextUnformatted(PhysicsConfigConfig::headerText);
    ImGui::Spacing();
    ImGui::TextUnformatted(PhysicsConfigConfig::inteText);
    ImGui::Separator();
    ImGui::Spacing();
}

void PhysicsConfigPanel::drawCombos() {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, PhysicsConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, PhysicsConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, PhysicsConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_Border, PhysicsConfigConfig::borderWhite);
    ImGui::PushStyleColor(ImGuiCol_Button, PhysicsConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PhysicsConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, PhysicsConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_Header, PhysicsConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, PhysicsConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, PhysicsConfigConfig::borderWhite);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, PhysicsConfigConfig::frameBorderSize);

    const float windowSizeX = ImGui::GetWindowWidth();
    const float comboWidth = windowSizeX * PhysicsConfigConfig::comboWidthScale;

    int currentInte = static_cast<int>(m_localPhysicsConfig.integrator);
    ImGui::SetNextItemWidth(comboWidth);
    if (ImGui::Combo(PhysicsConfigConfig::inteLabel, &currentInte, PhysicsConfigConfig::inteOptions)) {
        m_localPhysicsConfig.integrator = static_cast<PhysicsConfig::Integrator>(currentInte);
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(PhysicsConfigConfig::comboStyleColorCount);
}

void PhysicsConfigPanel::drawConstantInputs() {
    ImGui::TextUnformatted(PhysicsConfigConfig::constText);
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_FrameBg, PhysicsConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, PhysicsConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, PhysicsConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_Border, PhysicsConfigConfig::borderWhite);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, PhysicsConfigConfig::frameBorderSize);

    const float inputWidth = ImGui::GetWindowWidth() * PhysicsConfigConfig::comboWidthScale;

    ImGui::SetNextItemWidth(inputWidth);
    ImGui::InputDouble(PhysicsConfigConfig::gConstantLabel, &m_localPhysicsConfig.GConstant, 0.0, 0.0, PhysicsConfigConfig::highPrecisionFormat);

    ImGui::SetNextItemWidth(inputWidth);
    ImGui::InputDouble(PhysicsConfigConfig::deltaTimeLabel, &m_localPhysicsConfig.deltaTime, 0.0, 0.0, PhysicsConfigConfig::highPrecisionFormat);

    ImGui::SetNextItemWidth(inputWidth);
    ImGui::InputDouble(PhysicsConfigConfig::softeningLengthLabel, &m_localPhysicsConfig.softeningLength, 0.0, 0.0, PhysicsConfigConfig::medPrecisionFormat);

    ImGui::SetNextItemWidth(inputWidth);
    ImGui::InputDouble(PhysicsConfigConfig::thetaLabel, &m_localPhysicsConfig.theta, 0.0, 0.0, PhysicsConfigConfig::medPrecisionFormat);

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(PhysicsConfigConfig::inputStyleColorCount);
}
