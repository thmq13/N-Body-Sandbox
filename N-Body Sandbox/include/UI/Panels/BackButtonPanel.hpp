#pragma once

#include <Core/ApplicationState.hpp>
#include <UI/IPanel.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class BackButtonPanel : public IPanel {
    public:
        explicit BackButtonPanel(Core::ApplicationState nextState);
        ~BackButtonPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;

    private:
        Core::ApplicationState m_nextState;

        void setUpWindowAndStyle();
        void cleanUp();
    };
}