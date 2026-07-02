
#pragma once

#include <UI/IPanel.hpp>
#include <Core/Events.hpp> 

class SpawnerConfigPanel : public IPanel {
public:
    ~SpawnerConfigPanel() override = default;

    void draw(MessageBus& messageBus) override;

private:
    void setUpWindowAndStyle();
    void cleanUp();

    SpawnerConfig m_localSpawnerConfig{};
};
