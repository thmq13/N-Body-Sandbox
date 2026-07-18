#pragma once

#include <UI/IPanel.hpp>
#include <Core/AppState.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class BackButtonPanel : public IPanel {
    public:
        explicit BackButtonPanel(Core::AppState nextState);
        ~BackButtonPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;

    private:
        Core::AppState m_nextState;

        void setUpWindowAndStyle();
        void cleanUp();
    };
}