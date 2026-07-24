#pragma once

#include <array>
#include <string>

#include <UI/IPanel.hpp>
#include <Core/Message.hpp>
#include <UI/UITypes.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {

    class PhysicsPanel final : public IPanel {
    public:
        explicit PhysicsPanel(UIStorage& storage);
        ~PhysicsPanel() override = default;

        void Draw(Core::MessageBus& messageBus) override;
    private:
        void SetUpWindowAndStyle();
        void CleanUp();

        UIStoredSubModules& m_subStorage;
        UIActiveTarget m_activeTarget{};
    };

} // namespace NBody::UI
