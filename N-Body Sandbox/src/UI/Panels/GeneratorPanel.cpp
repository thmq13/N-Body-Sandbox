#include <UI/Panels/GeneratorPanel.hpp>

#include <cstddef>

#include <imgui.h>

#include <Core/MessageBus.hpp>
#include <Core/Message.hpp>
#include <UI/SchemaRenderer.hpp>

namespace NBody::UI {

    namespace {
        constexpr std::size_t generatorIdx{ static_cast<std::size_t>(Core::Module::Generator) };

        namespace Config {
            constexpr ImGuiWindowFlags windowFlags{
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus
            };

            constexpr int baseIdOffset{ 1000 };

            constexpr float widthScale{ 0.4f };
            constexpr float heightScale{ 0.40f };
            constexpr float offsetXScale{ 0.5f };
            constexpr float indentOffset{ 10.0f };
            constexpr float firstColumnScale{ 0.6f };

            constexpr float fontScale{ 1.75f };

            constexpr float comboWidthScale{ 0.35f };
            constexpr float inputWidthScale{ 0.25f };
            constexpr float displayWidthScale{ 0.25f };
            constexpr float frameBorderSize{ 1.0f };

            constexpr ImVec4 pureBlack{ 0.0f, 0.0f, 0.0f, 1.0f };
            constexpr ImVec4 mutedGray{ 0.2f, 0.2f, 0.2f, 1.0f };
            constexpr ImVec4 activeGray{ 0.3f, 0.3f, 0.3f, 1.0f };
            constexpr ImVec4 borderWhite{ 1.0f, 1.0f, 1.0f, 0.5f };
            constexpr ImVec4 retroWhite{ 1.0f, 1.0f, 1.0f, 1.0f };

            constexpr int styleColorCount{ 11 };
            constexpr int columnCount{ 2 };

            constexpr const char* windowName{ "Generator Panel" };
            constexpr const char* previewShapesText{ "Particle Preview List" };
            constexpr const char* addShapeText{ "> Add Shape <" };
            constexpr const char* removeShapeGlyph{ "X" };
        }
    }

    GeneratorPanel::GeneratorPanel(UIStorage& storage) 
        : m_subStorage(storage[generatorIdx])
    {
        m_activeTarget.resize(Core::generatorSubModuleCount);
    }

    void GeneratorPanel::draw(Core::MessageBus& messageBus) {
        setUpWindowAndStyle();

        const float windowWidth = ImGui::GetContentRegionAvail().x;

        ImGui::Columns(Config::columnCount, "ConfigColumns", true);
        ImGui::SetColumnWidth(0, Config::firstColumnScale * windowWidth);

        const std::size_t subCount = Core::generatorSubModuleCount;

        for (std::size_t subIdx = 0; subIdx < subCount; ++subIdx) {

            ImGui::PushID(static_cast<int>(generatorIdx * Config::baseIdOffset + subIdx));

            auto& targets = m_subStorage[subIdx];
            std::string& currentTarget = m_activeTarget[subIdx];

            if (targets.empty()) {
                currentTarget.clear();
            }
            else if (currentTarget.empty() || !targets.contains(currentTarget)) {
                currentTarget = targets.begin()->first;
            }

            ImGui::SetNextItemWidth(windowWidth * Config::comboWidthScale);
            if (ImGui::BeginCombo(Core::generatorSubModuleName[subIdx], currentTarget.c_str())) {
                for (const auto& [target, schemas] : targets) {
                    bool isSelected{ target == currentTarget };

                    if (ImGui::Selectable(target.c_str(), isSelected)) currentTarget = target;
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (!currentTarget.empty() && targets.contains(currentTarget)) {
                auto& schemas = targets[currentTarget];
                ImGui::Indent(Config::indentOffset);
                for (auto& schema : schemas) {
                    SchemaRenderer::drawInput(schema, windowWidth * Config::inputWidthScale);
                }
                ImGui::Unindent(Config::indentOffset);
                ImGui::Spacing();
            }

            ImGui::PopID();
        }
        ImGui::NextColumn();
        drawPreviewList(messageBus);

        cleanUp();
    }

    void GeneratorPanel::setUpWindowAndStyle() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        const float windowWidth = viewport->Size.x * Config::widthScale;
        const float windowHeight = viewport->Size.y * Config::heightScale;
        const float posY = viewport->Pos.y + (viewport->Size.y - windowHeight);

        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 15.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, Config::frameBorderSize);

        ImGui::Begin(Config::windowName, nullptr, Config::windowFlags);
        ImGui::PopStyleVar(2);

        ImGui::PushStyleColor(ImGuiCol_Text, Config::retroWhite);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, Config::pureBlack);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Config::mutedGray);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Config::activeGray);
        ImGui::PushStyleColor(ImGuiCol_Border, Config::borderWhite);
        ImGui::PushStyleColor(ImGuiCol_Button, Config::pureBlack);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Config::mutedGray);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Config::activeGray);
        ImGui::PushStyleColor(ImGuiCol_Header, Config::pureBlack);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Config::mutedGray);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, Config::activeGray);

        ImGui::SetWindowFontScale(Config::fontScale);
    }

    void GeneratorPanel::cleanUp() {
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor(Config::styleColorCount);
        ImGui::End();
    }

    void GeneratorPanel::drawPreviewList(Core::MessageBus& messageBus) {
        const float windowWidth = ImGui::GetContentRegionAvail().x;
        const float scrollingAreaHeight = ImGui::GetContentRegionAvail().y;

        ImGui::PushID(generatorIdx);
        ImGui::BeginChild("ScrollingPreviewList", ImVec2(0, scrollingAreaHeight), false, ImGuiWindowFlags_None);
        ImGui::SetWindowFontScale(Config::fontScale);

        ImGui::Text(Config::previewShapesText);

        bool requiresPreviewUpdate{ false };

        std::size_t shapeIdx = static_cast<std::size_t>(Core::GeneratorSubModule::Shape);
        std::string& activeShape = m_activeTarget[shapeIdx];

        if (ImGui::Button(Config::addShapeText)) {
            m_activeShapes.emplace_back(activeShape, m_subStorage[shapeIdx][activeShape]);
            requiresPreviewUpdate = true;
        }
        ImGui::Spacing();
        ImGui::Separator();

        for (auto it = m_activeShapes.begin(); it != m_activeShapes.end(); ) {
            ImGui::PushID(std::to_address(it));

            if (ImGui::Button(Config::removeShapeGlyph)) {
                it = m_activeShapes.erase(it);
                requiresPreviewUpdate = true;
                ImGui::PopID();
                continue;
            }
            ImGui::SameLine();
            ImGui::Text(it->shapeId.c_str());

            ImGui::Indent(Config::indentOffset);
            for (auto& schema : it->schemas) {
                SchemaRenderer::drawDisplay(schema);
            }
            ImGui::Unindent(Config::indentOffset);

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::PopID();
            ++it;
        }

        ImGui::EndChild();
        ImGui::PopID();

        if (requiresPreviewUpdate) {
            messageBus.publish(Core::CmdGeneratePreviewParticles{ m_activeShapes });
        }
    }
}

