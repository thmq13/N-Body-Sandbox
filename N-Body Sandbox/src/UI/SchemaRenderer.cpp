#include <UI/SchemaRenderer.hpp>

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <variant>

#include <imgui.h>

#include <Core/Message.hpp>
#include <Helpers/Constants.hpp>
#include <Helpers/Vec.hpp>
#include <raylib.h>

namespace NBody::UI::SchemaRenderer {

    bool drawInput(Core::ParameterSchema& schema, float fieldWidth) {
        ImGui::PushID(&schema);
        ImGui::SetNextItemWidth(fieldWidth);

        bool changed{ false };

        std::visit([&](auto& value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, bool>) {
                changed = ImGui::Checkbox(schema.label.c_str(),&value);
            }

            else if constexpr (std::is_same_v<T, int>) {
                ImGui::InputInt(schema.label.c_str(), &value, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
                changed = ImGui::IsItemDeactivatedAfterEdit();

                int min = std::holds_alternative<int>(schema.minValue)
                    ? std::get<int>(schema.minValue)
                    : Constant::Limit::intMin;

                int max = std::holds_alternative<int>(schema.maxValue)
                    ? std::get<int>(schema.maxValue)
                    : Constant::Limit::intMax;

                value = std::clamp(value, min, max);
            }

            else if constexpr (std::is_same_v<T, std::size_t>) {
                int tempVal = static_cast<int>(value);
                ImGui::InputInt(schema.label.c_str(), &tempVal, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    value = static_cast<std::size_t>(std::max(0, tempVal));
                    changed = true;
                }

                std::size_t min = std::holds_alternative<std::size_t>(schema.minValue)
                    ? std::get<std::size_t>(schema.minValue)
                    : 0;

                std::size_t max = std::holds_alternative<std::size_t>(schema.maxValue)
                    ? std::get<std::size_t>(schema.maxValue)
                    : Constant::Limit::sizeTMax;

                value = std::clamp(value, min, max);
            }

            else if constexpr (std::is_same_v<T, float>) {
                ImGui::InputFloat(schema.label.c_str(), &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
                changed = ImGui::IsItemDeactivatedAfterEdit();

                float min = std::holds_alternative<float>(schema.minValue)
                    ? std::get<float>(schema.minValue)
                    : Constant::Limit::floatMin;

                float max = std::holds_alternative<float>(schema.maxValue)
                    ? std::get<float>(schema.maxValue)
                    : Constant::Limit::floatMax;

                value = std::clamp(value, min, max);
            }

            else if constexpr (std::is_same_v<T, double>) {
                ImGui::InputDouble(schema.label.c_str(), &value, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
                changed = ImGui::IsItemDeactivatedAfterEdit();

                double min = std::holds_alternative<double>(schema.minValue)
                    ? std::get<double>(schema.minValue)
                    : Constant::Limit::doubleMin;

                double max = std::holds_alternative<double>(schema.maxValue)
                    ? std::get<double>(schema.maxValue)
                    : Constant::Limit::doubleMax;

                value = std::clamp(value, min, max);
            }

            else if constexpr (std::is_same_v<T, Math::Vec3>) {
                ImGui::InputScalarN(schema.label.c_str(), ImGuiDataType_Double, &value.x, 3, nullptr, nullptr, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
                changed = ImGui::IsItemDeactivatedAfterEdit();
            }

            else if constexpr (std::is_same_v<T, Color>) {
                float col[3] = {
                    value.r / 255.0f,
                    value.g / 255.0f,
                    value.b / 255.0f
                };

                ImGui::ColorEdit3(schema.label.c_str(), col);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    value.r = static_cast<unsigned char>(col[0] * 255.0f + 0.5f);
                    value.g = static_cast<unsigned char>(col[1] * 255.0f + 0.5f);
                    value.b = static_cast<unsigned char>(col[2] * 255.0f + 0.5f);
                    changed = true;
                }
            }

            else {
                ImGui::Text("Unsupported parameter type");
            }

        }, schema.value);

        ImGui::PopID();
        return changed;
    }

    void drawDisplay(const Core::ParameterSchema& schema) {
        ImGui::PushID(&schema);

        std::visit([&](const auto& value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, bool>) {
                ImGui::Checkbox(schema.label.c_str(), const_cast<bool*>(&value));
            }

            else if constexpr (std::is_same_v<T, int>) {
                ImGui::Text("%s: %d", schema.label.c_str(), value);
            }

            else if constexpr (std::is_same_v<T, std::size_t>) {
                ImGui::Text("%s: %zu", schema.label.c_str(), value);
            }

            else if constexpr (std::is_same_v<T, float>) {
                ImGui::Text("%s: %.3f", schema.label.c_str(), value);
            }

            else if constexpr (std::is_same_v<T, double>) {
                ImGui::Text("%s: %.6f", schema.label.c_str(), value);
            }

            else if constexpr (std::is_same_v<T, Math::Vec3>) {
                ImGui::Text("%s: (%.3f, %.3f, %.3f)",
                    schema.label.c_str(),
                    value.x,
                    value.y,
                    value.z);
            }

            else if constexpr (std::is_same_v<T, Color>) {
                float col[3] = {
                    value.r / 255.0f,
                    value.g / 255.0f,
                    value.b / 255.0f
                };

                ImGui::Text("%s:", schema.label.c_str());
                ImGui::SameLine();
                ImGui::ColorButton("##color", ImVec4(col[0], col[1], col[2], 1.0f));
            }

            else {
                ImGui::Text("%s: Unsupported parameter type", schema.label.c_str());
            }

        }, schema.value);

        ImGui::PopID();
    }

}