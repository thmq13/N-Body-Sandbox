#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include <AppState.hpp>
#include <Config.hpp>

struct CmdExitApplication {};

struct CmdRequestStateChange {
    AppState requestedState;
};

struct CmdSpawnPreviewCluster {
    SpawnerConfig::ClusterConfig cluster;
};

struct CmdRemovePreviewCluster {
    std::uint32_t id;
};

struct CmdUpdateSimulationConfig {
    ComputeConfig computeConfig;
    PhysicsConfig physicsConfig;
    SpawnerConfig spawnerConfig;
};

using SystemMessage = std::variant <
    CmdExitApplication,
    CmdRequestStateChange,
    CmdSpawnPreviewCluster,
    CmdRemovePreviewCluster,
    CmdUpdateSimulationConfig
>;