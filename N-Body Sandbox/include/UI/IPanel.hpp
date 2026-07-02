#pragma once

class MessageBus;

class IPanel {
public:
    virtual ~IPanel() = default;
    virtual void draw(MessageBus& messageBus) = 0;
};