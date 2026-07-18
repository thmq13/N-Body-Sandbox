#pragma once

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class IPanel {
    public:
        virtual ~IPanel() = default;
        virtual void draw(Core::MessageBus& messageBus) = 0;
    };
}
