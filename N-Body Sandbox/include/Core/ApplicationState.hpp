#pragma once

#include <cstdint>
#include <string_view>
#include <format>

namespace NBody::Core {
    enum class ApplicationState : std::uint8_t {
        Uninitialized = 0,
        TitleScreen,
        ModeSelection,
        RealTimeConfig,
        RealTimeRunning,
        PrecomputeConfig,
        PrecomputeRunning,
        PlaybackConfig,
        PlaybackRunning,
        TearDown
    };

    constexpr std::string_view ToString(ApplicationState state) noexcept {
        switch (state) {
        case ApplicationState::Uninitialized:      return "Uninitialized";
        case ApplicationState::TitleScreen:        return "TitleScreen";
        case ApplicationState::ModeSelection:      return "ModeSelection";
        case ApplicationState::RealTimeConfig:     return "RealTimeConfig";
        case ApplicationState::RealTimeRunning:    return "RealTimeRunning";
        case ApplicationState::PrecomputeConfig:   return "PrecomputeConfig";
        case ApplicationState::PrecomputeRunning:  return "PrecomputeRunning";
        case ApplicationState::PlaybackConfig:     return "PlaybackConfig";
        case ApplicationState::PlaybackRunning:    return "PlaybackRunning";
        case ApplicationState::TearDown:           return "TearDown";
        default:                                   return "Unknown";
        }
    }
}

template <>
struct std::formatter<NBody::Core::ApplicationState> : std::formatter<std::string_view> {
    auto format(NBody::Core::ApplicationState state, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(NBody::Core::ToString(state), ctx);
    }
};
