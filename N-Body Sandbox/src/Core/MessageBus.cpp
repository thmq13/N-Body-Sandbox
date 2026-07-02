#include <Core/MessageBus.hpp>

#include <variant>

void MessageBus::publish(SystemMessage message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(std::move(message));
}

bool MessageBus::isEmpty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

void MessageBus::dispatch() {
    std::queue<SystemMessage> localQueue{};
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::swap(localQueue, m_queue);
    }

    while (!localQueue.empty()) {
        const auto& message = localQueue.front();
        
        std::type_index messageTypeId = std::visit([](const auto& actualMessage) -> std::type_index {
            return typeid(actualMessage);
        }, message);

        std::vector<MessageCallback> callbacksToInvoke;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_subscribers.find(messageTypeId);
            if (it != m_subscribers.end()) {
                callbacksToInvoke = it->second;
            }
        }

        for (const auto& callback : callbacksToInvoke) {
            callback(message);
        }

        localQueue.pop();
    }
}
