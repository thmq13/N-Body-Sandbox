#pragma once

#include <vector>
#include <string>
#include <cstddef>

#include <UI/UITypes.hpp>
#include <UI/IPanel.hpp>
#include <Core/AppState.hpp>
#include <Core/Message.hpp>

class SimulationConfigPanel : public IPanel {
public:
    explicit SimulationConfigPanel(AppState nextState, UIStorage& storage);

    ~SimulationConfigPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:

    AppState m_nextState;

    UIStorage& m_storage;
    UIActiveTarget m_activeTargets{};

    std::vector<CmdGeneratePreviewParticles::Shape> m_activeShapes;

    void setUpWindowAndStyle();
    void cleanUp();

    void drawPreviewSection(MessageBus& messageBus);
    void drawApplyAndRunButton(MessageBus& messageBus);
    
};