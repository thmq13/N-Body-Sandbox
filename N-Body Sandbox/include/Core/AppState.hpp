#pragma once

#include <cstdint>

enum class AppState : uint8_t {
    TitleScreen,
    ModeSelection,
    RealTimeConfig,
    RealTimeRunning,
    PrecomputeConfig,
    PrecomputeRunning,
    PlaybackConfig,
    PlaybackRunning
};