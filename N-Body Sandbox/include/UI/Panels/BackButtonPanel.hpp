#pragma once

#include <Core/ApplicationState.hpp>
#include <UI/IPanel.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {

    class BackButtonPanel final : public IPanel {
    public:
        explicit BackButtonPanel(Core::ApplicationState nextState);
        ~BackButtonPanel() override = default;

        void Draw(Core::MessageBus& messageBus) override;

    private:
        Core::ApplicationState m_nextState;

        void SetUpWindowAndStyle();
        void CleanUp();
    };

} // namespace NBody::UI