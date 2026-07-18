#pragma once

#include <cstdint>

namespace NBody::Core {
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
}
