#pragma once

#include <UI/IPanel.hpp>
#include <Core/Events.hpp> 

class PhysicsConfigPanel : public IPanel {
public:
    ~PhysicsConfigPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    void setUpWindowAndStyle();
    void cleanUp();

    void drawMainTitle();
    void drawCombos(); // One combo for the integrators
    void drawConstantInputs(); // The 4 numbers

    PhysicsConfig m_localPhysicsConfig{};
};