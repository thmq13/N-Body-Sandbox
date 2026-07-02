#pragma once

#include <cstdint>

enum class UIState : uint8_t {
    TitleScreen,
    ModeSelection,
    RealTimeConfig,
    RealTimeRunning,
    PrecomputeConfig,
    PrecomputeRunning,
    PlaybackConfig,
    PlaybackRunning
};