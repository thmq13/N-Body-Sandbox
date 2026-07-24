#include <UI/Panels/PhysicsPanel.hpp>

#include <cstddef>
#include <cstdint>

#include <imgui.h>

#include <Core/MessageBus.hpp>
#include <Core/Message.hpp>
#include <UI/SchemaRenderer.hpp>

namespace NBody::UI {

    namespace {
        constexpr std::size_t physicsIdx{ static_cast<std::size_t>(Core::Module::Physics) };

        namespace Config {
            constexpr ImGuiWindowFlags windowFlags{
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus
            };

            constexpr int baseIdOffset{ 1000 };

            constexpr float widthScale{ 0.25f };
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

            constexpr const char* windowName{ "Physics Panel" };
        }
    }

    PhysicsPanel::PhysicsPanel(UIStorage& storage)
        : m_subStorage(storage[physicsIdx])
    {
        m_activeTarget.resize(Core::physicsSubModuleCount);
    }

    void PhysicsPanel::Draw(Core::MessageBus& messageBus) {
        SetUpWindowAndStyle();

        const float windowWidth = ImGui::GetContentRegionAvail().x;
        const std::size_t subCount = Core::physicsSubModuleCount;

        for (std::size_t subIdx = 0; subIdx < subCount; ++subIdx) {

            ImGui::PushID(static_cast<int>(physicsIdx * Config::baseIdOffset + subIdx));

            auto& targets = m_subStorage[subIdx];
            std::string& currentTarget = m_activeTarget[subIdx];
            std::string previousTarget{ currentTarget };

            if (targets.empty()) {
                currentTarget.clear();
            }
            else if (currentTarget.empty() || !targets.contains(currentTarget)) {
                currentTarget = targets.begin()->first;
            }

            ImGui::SetNextItemWidth(windowWidth * Config::comboWidthScale);
            if (ImGui::BeginCombo(Core::physicsSubModuleName[subIdx], currentTarget.c_str())) {
                for (const auto& [target, schemas] : targets) {
                    bool isSelected{ target == currentTarget };

                    if (ImGui::Selectable(target.c_str(), isSelected)) currentTarget = target;
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (previousTarget != currentTarget) {
                messageBus.Publish(Core::CmdSetActiveOption{
                    static_cast<std::uint32_t>(physicsIdx),
                    static_cast<std::uint32_t>(subIdx),
                    currentTarget
                });
            }

            if (!currentTarget.empty() && targets.contains(currentTarget)) {
                auto& schemas = targets[currentTarget];
                ImGui::Indent(Config::indentOffset);
                for (auto& schema : schemas) {
                    bool valueChanged = SchemaRenderer::DrawInput(schema, windowWidth * Config::inputWidthScale);
                    if (valueChanged) {
                        messageBus.Publish(Core::CmdSetConfig{
                            static_cast<std::uint32_t>(physicsIdx),
                            static_cast<std::uint32_t>(subIdx),
                            currentTarget,
                            schema
                        });
                    }
                }
                ImGui::Unindent(Config::indentOffset);
                ImGui::Spacing();
            }

            ImGui::PopID();
        }
        ImGui::NextColumn();

        CleanUp();
    }

    void PhysicsPanel::SetUpWindowAndStyle() {
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

    void PhysicsPanel::CleanUp() {
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor(Config::styleColorCount);
        ImGui::End();
    }
}

