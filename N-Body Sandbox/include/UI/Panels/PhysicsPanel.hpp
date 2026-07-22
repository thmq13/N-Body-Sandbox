#pragma once

#include <array>
#include <string>

#include <UI/IPanel.hpp>
#include <Core/Message.hpp>
#include <UI/UITypes.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class PhysicsPanel : public IPanel {
    public:
        explicit PhysicsPanel(UIStorage& storage);
        ~PhysicsPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;
    private:
        void setUpWindowAndStyle();
        void cleanUp();

        UIStoredSubModules& m_subStorage;
        UIActiveTarget m_activeTarget{};
    };
}
