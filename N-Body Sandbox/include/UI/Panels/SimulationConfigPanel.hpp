#pragma once

#include <vector>

#include <UI/UITypes.hpp>
#include <UI/IPanel.hpp>
#include <Core/AppState.hpp>
#include <Core/Message.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class SimulationConfigPanel : public IPanel {
    public:
        explicit SimulationConfigPanel(Core::AppState nextState, UIStorage& storage);

        ~SimulationConfigPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;

    private:

        Core::AppState m_nextState;

        UIStorage& m_storage;
        UIActiveTarget m_activeTargets{};

        std::vector<Core::CmdGeneratePreviewParticles::Shape> m_activeShapes;

        void setUpWindowAndStyle();
        void cleanUp();

        void drawPreviewSection(Core::MessageBus& messageBus);
        void drawApplyAndRunButton(Core::MessageBus& messageBus);

    };
}
