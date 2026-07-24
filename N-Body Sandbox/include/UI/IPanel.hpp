#pragma once

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {

    class IPanel {
    public:
        virtual ~IPanel() = default;
        virtual void Draw(Core::MessageBus& messageBus) = 0;
    };

} // namespace NBody::UI


