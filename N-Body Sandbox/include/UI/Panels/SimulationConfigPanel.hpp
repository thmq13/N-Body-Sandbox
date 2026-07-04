#pragma once

#include <cstddef>

#include <UI/IPanel.hpp>
#include <Core/Events.hpp> 
#include <AppState.hpp>
#include <Config.hpp>

class SimulationConfigPanel : public IPanel {
public:
    SimulationConfigPanel(AppState targetRunningState)
        : m_targetRunningState(targetRunningState) {}

    ~SimulationConfigPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    void setUpWindowAndStyle();
    void cleanUp();

    void drawComputeSection();
    void drawPhysicsSection();
    void drawSpawnerSection(MessageBus& messageBus);
    void drawApplyAndRunButton(MessageBus& messageBus);

    ComputeConfig m_localComputeConfig{};
    PhysicsConfig m_localPhysicsConfig{};
    SpawnerConfig m_localSpawnerConfig{};

    AppState m_targetRunningState;

    SpawnerConfig::Shape m_pendingShape{ SpawnerConfig::Shape::UniformSphere };
    SpawnerConfig::ShapeParams m_pendingParams;
    std::size_t m_nextClusterId{ 0 };
};