#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <type_traits>
#include <array>
#include <algorithm>

#include <imgui.h>
#include <raylib.h>

#include <Core/AppState.hpp>
#include <Helpers/Vec.hpp>
#include <Helpers/Constants.hpp>

// --- GENERAL TYPES FOR COMMUNICATION ---

using ParameterValue = std::variant<bool, int, std::size_t, float, double, std::string, Vec3, Color>;

struct ParameterSchema {
    std::string label{};
    ParameterValue value{};
    ParameterValue minValue{};
    ParameterValue maxValue{};

    void drawInput(float fieldWidth = 100.0f) {
        ImGui::PushID(this);
        ImGui::SetNextItemWidth(fieldWidth);

        std::visit([this](auto& actualValue) {
            using T = std::decay_t<decltype(actualValue)>;

            if constexpr (std::is_same_v<T, bool>) {
                ImGui::Checkbox(label.c_str(), &actualValue);
            }
            else if constexpr (std::is_same_v<T, int>) {
                ImGui::InputInt(label.c_str(), &actualValue);

                int min = std::holds_alternative<int>(minValue) ? std::get<int>(minValue) : Limit::intMin;
                int max = std::holds_alternative<int>(maxValue) ? std::get<int>(maxValue) : Limit::intMax;
                actualValue = std::clamp(actualValue, min, max);
            }
            else if constexpr (std::is_same_v<T, std::size_t>) {
                int tempVal = static_cast<int>(actualValue);
                if (ImGui::InputInt(label.c_str(), &tempVal)) {
                    actualValue = static_cast<std::size_t>(std::max(0, tempVal));
                }

                std::size_t min = std::holds_alternative<std::size_t>(minValue) ? std::get<std::size_t>(minValue) : 0;
                std::size_t max = std::holds_alternative<std::size_t>(maxValue) ? std::get<std::size_t>(maxValue) : Limit::sizeTMax;
                actualValue = std::clamp(actualValue, min, max);
            }
            else if constexpr (std::is_same_v<T, float>) {
                ImGui::InputFloat(label.c_str(), &actualValue, 0.0f, 0.0f, "%.3f");

                float min = std::holds_alternative<float>(minValue) ? std::get<float>(minValue) : Limit::floatMin;
                float max = std::holds_alternative<float>(maxValue) ? std::get<float>(maxValue) : Limit::floatMax;
                actualValue = std::clamp(actualValue, min, max);
            }
            else if constexpr (std::is_same_v<T, double>) {
                ImGui::InputDouble(label.c_str(), &actualValue, 0.0, 0.0, "%.6f");

                double min = std::holds_alternative<double>(minValue) ? std::get<double>(minValue) : Limit::doubleMin;
                double max = std::holds_alternative<double>(maxValue) ? std::get<double>(maxValue) : Limit::doubleMax;
                actualValue = std::clamp(actualValue, min, max);
            }
            else if constexpr (std::is_same_v<T, Vec3>) {
                ImGui::InputScalarN(label.c_str(), ImGuiDataType_Double, &actualValue.x, 3, nullptr, nullptr, "%.3f");
            }
            else if constexpr (std::is_same_v<T, Color>) {
                float col[3] = {
                    actualValue.r / 255.0f,
                    actualValue.g / 255.0f,
                    actualValue.b / 255.0f
                };
                if (ImGui::ColorEdit3(label.c_str(), col)) {
                    actualValue.r = static_cast<unsigned char>(col[0] * 255.0f + 0.5f);
                    actualValue.g = static_cast<unsigned char>(col[1] * 255.0f + 0.5f);
                    actualValue.b = static_cast<unsigned char>(col[2] * 255.0f + 0.5f);
                }
            }
            else {
                ImGui::Text("Unsupported parameter type");
            }

            }, value);

        ImGui::PopID();
    }

    void drawDisplay() const {
        ImGui::PushID(this);

        std::visit([this](const auto& actualValue) {
            using T = std::decay_t<decltype(actualValue)>;

            if constexpr (std::is_same_v<T, bool>) {
                ImGui::Checkbox(label.c_str(), const_cast<bool*>(&actualValue));
            }
            else if constexpr (std::is_same_v<T, int>) {
                ImGui::Text("%s: %d", label.c_str(), actualValue);
            }
            else if constexpr (std::is_same_v<T, std::size_t>) {
                ImGui::Text("%s: %zu", label.c_str(), actualValue);
            }
            else if constexpr (std::is_same_v<T, float>) {
                ImGui::Text("%s: %.3f", label.c_str(), actualValue);
            }
            else if constexpr (std::is_same_v<T, double>) {
                ImGui::Text("%s: %.6f", label.c_str(), actualValue);
            }
            else if constexpr (std::is_same_v<T, Vec3>) {
                ImGui::Text("%s: (%.3f, %.3f, %.3f)",
                    label.c_str(),
                    actualValue.x,
                    actualValue.y,
                    actualValue.z);
            }
            else if constexpr (std::is_same_v<T, Color>) {
                float col[3] = {
                    actualValue.r / 255.0f,
                    actualValue.g / 255.0f,
                    actualValue.b / 255.0f
                };

                ImGui::Text("%s:", label.c_str());
                ImGui::SameLine();
                ImGui::ColorButton("##color", ImVec4(col[0], col[1], col[2], 1.0f));
            }
            else {
                ImGui::Text("%s: Unsupported parameter type", label.c_str());
            }

            }, value);

        ImGui::PopID();
    }
};

// Don't ask me why
enum class Module : std::uint32_t { Physics, Generator, Count };
inline constexpr std::size_t moduleCount = static_cast<std::size_t>(Module::Count);
inline const std::array<std::string, moduleCount> moduleNames{ "Physics", "Generator" };

enum class PhysicsSubModule : std::uint32_t { GravitySolver, Integrator, Count };
enum class GeneratorSubModule : std::uint32_t { Shape, Count };

inline const std::array<std::size_t, moduleCount> subModuleCount{ 
    static_cast<std::size_t>(PhysicsSubModule::Count),
    static_cast<std::size_t>(GeneratorSubModule::Count)
};

inline const std::array<std::vector<std::string>, moduleCount> subModuleNames{
    std::vector<std::string>{ "Gravity Solver", "Integrator" },
    std::vector<std::string>{ "Shape" }
};


// --- LIST OF COMMANDS ---


// System level
struct CmdExitApplication {
};

struct CmdRequestStateChange {
    AppState requestedState;
};


// UI communication
struct CmdRequestSchemas {
};

struct CmdSendSchemas {
    std::uint32_t moduleId;
    std::uint32_t subModuleId;
    std::string senderId;
    std::vector<ParameterSchema> schemas;
};

struct CmdSetActiveOption {
    std::uint32_t moduleId;
    std::uint32_t subModuleId;
    std::string activeId;
};

struct CmdSetConfig {
    std::uint32_t moduleId;
    std::uint32_t subModuleId;
    std::string targetId;
    std::vector<ParameterSchema> schemas;
};

// Misc
struct CmdGeneratePreviewParticles {
    struct Shape {
        std::string shapeId;
        std::vector<ParameterSchema> schemas;
    };
    std::vector<Shape> shapes;
};

struct CmdSetRandomizeSeed {
    std::uint64_t seed;
};


// --- THE FINAL VARIANT ---
using SystemMessage = std::variant <
    CmdExitApplication,
    CmdRequestStateChange,

    CmdRequestSchemas,
    CmdSendSchemas,
    CmdSetActiveOption,
    CmdSetConfig,

    CmdGeneratePreviewParticles,
    CmdSetRandomizeSeed
>;