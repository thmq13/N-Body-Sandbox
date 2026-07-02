#pragma once

#include <queue>
#include <mutex>    
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <any>

#include <Core/Events.hpp>

class MessageBus {
public:
    using MessageCallback = std::function<void(const SystemMessage&)>;

    MessageBus() = default;
    ~MessageBus() = default;

    MessageBus(const MessageBus&) = delete;
    MessageBus& operator=(const MessageBus&) = delete;
    MessageBus(MessageBus&&) = delete;
    MessageBus& operator=(MessageBus&&) = delete;

    template <typename T>
    void subscribe(MessageCallback callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_subscribers[typeid(T)].push_back(std::move(callback));
    }

    void publish(SystemMessage message);

    [[nodiscard]] bool isEmpty() const;

    void dispatch();

private:
    
    std::queue<SystemMessage> m_queue;

    std::unordered_map<std::type_index, std::vector<MessageCallback>> m_subscribers;

    mutable std::mutex m_mutex;
};