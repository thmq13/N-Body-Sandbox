#pragma once

#include <variant>
#include <vector>

#include <AppState.hpp>
#include <Config.hpp>

struct CmdExitApplication {};

struct CmdRequestStateChange {
    AppState requestedState;
};

//struct CmdSpawnCluster

struct CmdUpdateSimulationConfig {
    ComputeConfig computeConfig;
    PhysicsConfig physicsConfig;
    SpawnerConfig spawnerConfig;
};

using SystemMessage = std::variant <
    CmdExitApplication,
    CmdRequestStateChange,
    CmdUpdateSimulationConfig
>;