#include "UI/Panels/SimulationConfigPanel.hpp"

#include <variant>
#include <cstdint>

#include <imgui.h>

#include <Core/Message.hpp>
#include <Core/MessageBus.hpp>
#include <UI/SchemaRenderer.hpp>

namespace {
    namespace SimulationConfigConfig {
        constexpr ImGuiDataType size_t_type = (sizeof(size_t) == 8) ? ImGuiDataType_U64 : ImGuiDataType_U32;

        constexpr ImGuiWindowFlags windowFlags{
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus
        };

        constexpr float widthScale{ 1.00f };       
        constexpr float heightScale{ 0.40f };      
        constexpr float offsetX{ 0.0f };

        constexpr float defaultFontScale{ 1.75f };
        constexpr float moduleFontScale{ 2.0f };
        constexpr float previewShapesFontScale{ 1.5f };

        constexpr float comboWidthScale{ 0.15f };
        constexpr float inputWidthScale{ 0.10f };
        constexpr float displayWidthScale{ 0.10f };
        constexpr float frameBorderSize{ 1.0f };
        constexpr float applyWithScale{ 0.25f };

        constexpr ImVec4 pureBlack{ 0.0f, 0.0f, 0.0f, 1.0f };
        constexpr ImVec4 mutedGray{ 0.2f, 0.2f, 0.2f, 1.0f };
        constexpr ImVec4 activeGray{ 0.3f, 0.3f, 0.3f, 1.0f };
        constexpr ImVec4 borderWhite{ 1.0f, 1.0f, 1.0f, 0.5f };
        constexpr ImVec4 retroWhite{ 1.0f, 1.0f, 1.0f, 1.0f };

        constexpr int styleColorCount{ 11 };

        constexpr const char* windowName{ "Simulation Config Panel" };
        constexpr const char* previewShapesText{ "Particle Preview List" };
        constexpr const char* applyText{ "[ APPLY AND START SIMULATION ]" };
        constexpr const char* addShapeText{ "> Add Shape <" };
        constexpr const char* noShapesText{ "No active clusters." };
        constexpr const char* removeShapeGlyph{ "X" };

        constexpr const char* highPrecisionFormat{ "%.6f" };
        constexpr const char* medPrecisionFormat{ "%.4f" };
    }
}

SimulationConfigPanel::SimulationConfigPanel(AppState nextState, UIStorage& storage)
    : m_nextState(nextState),
    m_storage(storage)
{
    for (std::size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx) {
        m_activeTargets[moduleIdx].resize(subModuleCount[moduleIdx]);
    }
}

void SimulationConfigPanel::draw(MessageBus& messageBus) {

    setUpWindowAndStyle();

    const float windowWidth = ImGui::GetContentRegionAvail().x;

    ImGui::Columns(3, "ConfigColumns", true);
    ImGui::SetColumnWidth(0, windowWidth / 3.0f);
    ImGui::SetColumnWidth(1, windowWidth / 3.0f);

    for (std::size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx) {

        ImGui::SetWindowFontScale(SimulationConfigConfig::moduleFontScale);
        ImGui::Text(moduleNames[moduleIdx].c_str());
        ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);

        const std::size_t currentSubModuleCount = subModuleCount[moduleIdx];
        for (std::size_t subModuleIdx = 0; subModuleIdx < currentSubModuleCount; ++subModuleIdx) {
            ImGui::PushID(static_cast<int>(moduleIdx * 1000 + subModuleIdx));

            auto& targets = m_storage[moduleIdx][subModuleIdx];
            std::string& currentTargetId = m_activeTargets[moduleIdx][subModuleIdx];
            
            if (targets.empty()) {
                currentTargetId.clear();
            }
            else if (currentTargetId.empty() || !targets.contains(currentTargetId)) {
                currentTargetId = targets.begin()->first;
            }

            // Combo
            ImGui::SetNextItemWidth(windowWidth * SimulationConfigConfig::comboWidthScale);
            if (ImGui::BeginCombo(subModuleNames[moduleIdx][subModuleIdx].c_str(), currentTargetId.c_str())) {
                for (const auto& [targetId, schemas] : targets) {
                    bool isSelected{ targetId == currentTargetId };

                    if (ImGui::Selectable(targetId.c_str(), isSelected)) {
                        currentTargetId = targetId;
                    }   

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // Draw schemas for the active target of the combo
            if (!currentTargetId.empty() && targets.contains(currentTargetId)) {
                auto& schemas = targets[currentTargetId];
                ImGui::Indent(10.0f);
                for (auto& schema : schemas) {
                    SchemaRenderer::drawInput(schema, windowWidth * SimulationConfigConfig::inputWidthScale);
                }
                ImGui::Unindent(10.0f);
                ImGui::Spacing();
            }

            ImGui::PopID();
        }
        ImGui::NextColumn();
    }    

    drawPreviewSection(messageBus);
    ImGui::Columns(1);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    drawApplyAndRunButton(messageBus);

    cleanUp();
}

void SimulationConfigPanel::setUpWindowAndStyle() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float windowWidth = viewport->Size.x * SimulationConfigConfig::widthScale;
    const float windowHeight = viewport->Size.y * SimulationConfigConfig::heightScale;
    const float posY = viewport->Pos.y + (viewport->Size.y - windowHeight);

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + SimulationConfigConfig::offsetX, posY));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, SimulationConfigConfig::frameBorderSize);

    ImGui::Begin(SimulationConfigConfig::windowName, nullptr, SimulationConfigConfig::windowFlags);

    ImGui::PopStyleVar(2);

    ImGui::PushStyleColor(ImGuiCol_Text, SimulationConfigConfig::retroWhite);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, SimulationConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, SimulationConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, SimulationConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_Border, SimulationConfigConfig::borderWhite);
    ImGui::PushStyleColor(ImGuiCol_Button, SimulationConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SimulationConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, SimulationConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_Header, SimulationConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, SimulationConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, SimulationConfigConfig::activeGray);
}

void SimulationConfigPanel::cleanUp() {
    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);
    ImGui::PopStyleColor(SimulationConfigConfig::styleColorCount);
    ImGui::End();
}


void SimulationConfigPanel::drawPreviewSection(MessageBus& messageBus) {

    const float windowWidth = ImGui::GetContentRegionAvail().x;
    const float scrollingAreaHeight = ImGui::GetContentRegionAvail().y - 80.0f;

    ImGui::PushID(static_cast<int>(moduleCount));
    ImGui::BeginChild("ScrollingColumn", ImVec2(0, scrollingAreaHeight), false, ImGuiWindowFlags_None);

    ImGui::SetWindowFontScale(SimulationConfigConfig::moduleFontScale);
    ImGui::Text(SimulationConfigConfig::previewShapesText);
    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);

    bool requiresPreviewUpdate{ false };

    std::size_t generatorIdx = static_cast<std::size_t>(Module::Generator);
    std::size_t shapeIdx = static_cast<std::size_t>(GeneratorSubModule::Shape);
    std::string& activeShapeId = m_activeTargets[generatorIdx][shapeIdx];

    if (ImGui::Button(SimulationConfigConfig::addShapeText)) {
        m_activeShapes.emplace_back(activeShapeId, m_storage[generatorIdx][shapeIdx][activeShapeId]);
        requiresPreviewUpdate = true;
    }
    ImGui::Spacing();
    ImGui::Separator();

    ImGui::SetWindowFontScale(SimulationConfigConfig::previewShapesFontScale);

    for (auto it = m_activeShapes.begin(); it != m_activeShapes.end(); ) {
        ImGui::PushID(std::to_address(it));

        if (ImGui::Button(SimulationConfigConfig::removeShapeGlyph)) {
            it = m_activeShapes.erase(it);
            requiresPreviewUpdate = true;
            ImGui::PopID();
            continue;
        }
        ImGui::SameLine();
        ImGui::Text("%s", it->shapeId.c_str());

        ImGui::Indent(10.0f);
        for (auto& schema : it->schemas) {
            SchemaRenderer::drawDisplay(schema);
        }
        ImGui::Unindent(10.0f);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::PopID();
        ++it;
    }
    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);

    ImGui::EndChild();
    ImGui::PopID();

    if (requiresPreviewUpdate) {
        messageBus.publish(CmdGeneratePreviewParticles{m_activeShapes});
    }
}

void SimulationConfigPanel::drawApplyAndRunButton(MessageBus& messageBus) {
    const float windowWidth = ImGui::GetContentRegionAvail().x;
    const float buttonWidth = windowWidth * SimulationConfigConfig::applyWithScale;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (windowWidth - buttonWidth) * 0.5f);
    if (ImGui::Button(SimulationConfigConfig::applyText)) {
        for (std::size_t moduleIdx = 0; moduleIdx < moduleCount; ++moduleIdx) {

            std::size_t currentSubModuleCount = subModuleCount[moduleIdx];

            for (std::size_t subModuleIdx = 0; subModuleIdx < currentSubModuleCount; ++subModuleIdx) {

                messageBus.publish(CmdSetActiveOption{
                    static_cast<std::uint32_t>(moduleIdx),
                    static_cast<std::uint32_t>(subModuleIdx),
                    m_activeTargets[moduleIdx][subModuleIdx]
                });

                auto& targets = m_storage[moduleIdx][subModuleIdx];
                for (auto& [targetId, schemas] : targets) {
                    messageBus.publish(CmdSetConfig{
                        static_cast<std::uint32_t>(moduleIdx),
                        static_cast<std::uint32_t>(subModuleIdx),
                        targetId,
                        schemas
                    });
                }
            }
        }
        messageBus.publish(CmdRequestStateChange{ m_nextState });
    }
}
