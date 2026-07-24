#include <Core/MessageBus.hpp>

#include <Core/Message.hpp>

import std;

namespace NBody::Core {

    void MessageBus::Publish(SystemMessage message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(message));
    }

    bool MessageBus::IsEmpty() const noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void MessageBus::Dispatch() {
        std::queue<SystemMessage> localQueue{};
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(localQueue, m_queue);
        }

        while (!localQueue.empty()) {
            const auto& message = localQueue.front();

            std::type_index messageTypeId = std::visit([](const auto& msg) -> std::type_index {
                return typeid(msg);
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

} // namespace NBody::Core

