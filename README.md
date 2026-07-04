# N-Body Sandbox

> *"We are but dust pulling at each other in the void, waiting for the inevitable collapse."*

**N-Body Sandbox** is a real-time gravitational N-body simulator and sandbox, written in modern C++20. It renders live in 3D with [raylib](https://www.raylib.com/) and is driven through a [Dear ImGui](https://github.com/ocornut/imgui) interface, letting you compose scenes out of multiple clusters of bodies, tune the physics and force algorithm, and watch gravity take over.

<p align="center">
  <img src="Preview/Screenshot 2026-07-01 164515.png" width="32%" />
  <img src="Preview/Screenshot 2026-07-01 164541.png" width="32%" />
  <img src="Preview/Screenshot 2026-07-01 164557.png" width="32%" />
</p>

## Table of Contents

- [Features](#features)
- [Project Status](#project-status)
- [Tech Stack](#tech-stack)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

## Features

**Simulation modes**
- **Real-Time** — configure a scene and watch it simulate live
- **Precompute** — run a simulation ahead of time and store the result
- **Playback** — replay a precomputed run

**Scene composition** — build a scene out of any number of independently configured clusters:
| Shape | Parameters |
|---|---|
| Uniform Sphere | radius, total mass, body count |
| Plummer sphere | scale radius, total mass, body count |
| Disk Galaxy | radius, thickness, total mass, body count |

**Physics configuration**
- Gravitational constant, timestep, softening length, and Barnes-Hut θ (theta)
- Selectable integrator: Euler, Verlet (default), or RK4

**Compute configuration**
- Force algorithm: Direct N² or Barnes-Hut
- Target hardware: CPU or GPU
- Optional OpenMP and SIMD acceleration toggles

**Decoupled, threaded architecture**
- An event/message bus decouples the UI, physics, and rendering subsystems
- The physics engine runs on its own worker thread so simulation stepping never blocks the UI or the renderer
- Navigation between screens (title → mode select → per-mode config → running view) is entirely data-driven off a single app-state enum

## Project Status

This project is under active, early-stage development — treat it as a work-in-progress architecture sandbox as much as a physics sandbox. As of now:

**Working**
- Application shell: windowing, camera, and the raylib + Dear ImGui render pipeline
- Message bus and a threaded `PhysicsEngine` skeleton (subscribes to config/state messages, idles/wakes correctly)
- Full navigation shell: title screen, mode selection, and the Precompute configuration panel (compute / physics / spawner sections, shown in the screenshots above)

**Not yet implemented**
- The actual gravity solver and integrators — `ISolver`/`IIntegrator` interfaces exist, but the worker loop's call sites are stubbed out
- Barnes-Hut tree construction, the GPU compute path, and the OpenMP/SIMD paths
- Rendering of simulated bodies
- Configuration panels for Real-Time and Playback modes (currently only Precompute has one registered)
- `SimulationStateBuffer`, used for precompute/playback persistence, is currently an empty stub

In short: if you clone and run this today, you'll get a fully navigable UI shell around a simulation that doesn't compute or draw physics yet.

## Tech Stack

- **Language:** C++20
- **Rendering:** [raylib](https://www.raylib.com/) (vendored in `ext/raylib`)
- **UI:** [Dear ImGui](https://github.com/ocornut/imgui) via [rlImGui](https://github.com/raylib-extras/rlImGui) (vendored in `ext/`)
- **Build system:** Visual Studio 2022 solution (`.sln` / `.vcxproj`), platform toolset v143
- **Target platform:** Windows (Win32 and x64 configurations)

## Getting Started

### Prerequisites
- Windows 10/11
- Visual Studio 2022 with the **Desktop development with C++** workload

### Build & Run
```bash
git clone https://github.com/thmq13/N-Body-Sandbox.git
```
1. Open `N-Body Sandbox.sln` in Visual Studio 2022.
2. Choose a configuration (`Debug` or `Release`) and platform (`x64` recommended).
3. Build (`Ctrl+Shift+B`) and run (`F5`).

All third-party dependencies (raylib, Dear ImGui, rlImGui) are vendored under `ext/`, so no package manager (vcpkg, CMake, etc.) setup is required — the solution builds as-is.

> **Note:** the renderer launches fullscreen at your primary monitor's resolution by default. Use the **exit** button on the title screen to quit.

## Project Structure

```
N-Body Sandbox/
├── include/
│   ├── Core/                    # Message bus + event/message definitions
│   ├── Physics/                 # PhysicsEngine, ISolver / IIntegrator interfaces
│   ├── RenderingEngine/          # raylib + Dear ImGui render pipeline
│   ├── SimulationStateBuffer/    # (stub) shared buffer for precompute/playback
│   ├── UI/
│   │   └── Panels/               # TitleScreen, ModeSelection, SimulationConfig, BackButton
│   ├── AppState.hpp               # App-state enum driving UI + physics sync
│   ├── Config.hpp                 # ComputeConfig / PhysicsConfig / SpawnerConfig
│   └── NBodySandboxApplication.hpp
├── src/                          # Implementations mirroring include/
├── ext/                          # Vendored deps: raylib, Dear ImGui, rlImGui
├── Preview/                       # Screenshots used in this README
└── N-Body Sandbox.sln / .vcxproj
```

## Architecture

`NBodySandboxApplication` owns four subsystems — `MessageBus`, `PhysicsEngine`, `RenderingEngine`, `UIManager` — plus a shared `SimulationStateBuffer`.

- **MessageBus** — a lightweight pub/sub bus. Any subsystem can `subscribe<T>` to a `SystemMessage` variant (`CmdExitApplication`, `CmdRequestStateChange`, `CmdUpdateSimulationConfig`, …) and `publish()` to notify the others.
- **PhysicsEngine** — runs on its own `std::jthread`, pulling messages off a thread-safe mailbox. It steps the simulation while the app state is one of the `*Running` states, and otherwise sleeps on a condition variable instead of busy-waiting.
- **UIManager** — maps each `AppState` to the set of `IPanel`s that should be drawn for it (e.g. `TitleScreenPanel` only during `AppState::TitleScreen`), so the entire screen-navigation flow is data, not control flow.
- **RenderingEngine** — wraps raylib window/camera setup and Dear ImGui frame begin/end via rlImGui.

The main loop (`NBodySandboxApplication::executeMasterLoop`) is intentionally simple: dispatch pending bus messages, begin the frame, let the `UIManager` draw whatever panels are active, end the frame — repeat until a `CmdExitApplication` message arrives.

## Roadmap

- [ ] Direct N² solver + Euler/Verlet/RK4 integrators
- [ ] Barnes-Hut tree construction and θ-based approximation
- [ ] GPU compute path
- [ ] OpenMP / SIMD acceleration for the CPU path
- [ ] Rendering of simulated bodies/clusters
- [ ] Precompute mode: simulate and persist to `SimulationStateBuffer`
- [ ] Playback mode: scrub and replay a precomputed run
- [ ] Real-Time and Playback configuration panels
- [ ] Cross-platform build (currently Visual Studio / Windows only)

## Contributing

This is currently a solo, early-stage project. Issues and pull requests are welcome — the message-bus-plus-panel-per-state design is meant to make it easy to drop in a new solver, integrator, or screen without touching unrelated code.

## License

No license file is currently included in this repository. Until one is added, all rights are reserved by the author.
