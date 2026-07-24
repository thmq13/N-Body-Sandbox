#pragma once

#include <functional>
#include <mutex>    
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Core/Message.hpp>

namespace NBody::Core {
    class MessageBus final {
    public:
        using MessageCallback = std::function<void(const SystemMessage&)>;

        MessageBus() noexcept = default;
        ~MessageBus() = default;

        MessageBus(const MessageBus&) = delete;
        MessageBus& operator=(const MessageBus&) = delete;
        MessageBus(MessageBus&&) noexcept = delete;
        MessageBus& operator=(MessageBus&&) noexcept= delete;

        template <typename T>
        void subscribe(MessageCallback callback) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_subscribers[typeid(T)].push_back(std::move(callback));
        }

        void publish(SystemMessage message);

        [[nodiscard]] bool isEmpty() const noexcept;

        void dispatch();

    private:

        std::queue<SystemMessage> m_queue{};

        std::unordered_map<std::type_index, std::vector<MessageCallback>> m_subscribers{};

        mutable std::mutex m_mutex;
    };

    template <typename Derived>
    class MessageSubscriber {
    public:
        template <typename... MessageTypes>
        void SubscribeToMessages() {
            (static_cast<Derived*>(this)->template SubscribeToMessage<MessageTypes>(), ...);
        }
    };
}
