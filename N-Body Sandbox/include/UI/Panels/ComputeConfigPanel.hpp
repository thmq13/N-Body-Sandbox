#pragma once

#include <UI/IPanel.hpp>
#include <Core/Events.hpp> 

class ComputeConfigPanel : public IPanel {
public:
    ~ComputeConfigPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    void setUpWindowAndStyle();
    void cleanUp();

    void drawMainTitle();
    void drawCombos();
    void drawCheckBoxes();

    ComputeConfig m_localComputeConfig{};
};