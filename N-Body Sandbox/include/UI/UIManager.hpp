#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <Core/Events.hpp>
#include <UI/IPanel.hpp>
#include <AppState.hpp>

class MessageBus;

class UIManager {
public:
    explicit UIManager(MessageBus& messageBus);

    ~UIManager() = default;

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    UIManager(UIManager&&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    void draw(AppState state);

private:
    std::unordered_map < AppState, std::vector<IPanel*> > m_statePanels{};
    std::vector<std::unique_ptr<IPanel>> m_ownedPanels{};

    MessageBus& m_messageBus;

    void registerPanels();

    template<typename T, typename... Args>
    T* createPanel(AppState state, Args&&... args) {
        static_assert(std::is_base_of_v<IPanel, T>, "T must derive from IPanel");

        auto panel = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = panel.get();

        m_ownedPanels.push_back(std::move(panel));
        m_statePanels[state].push_back(ptr);

        return ptr;
    }

    void handleMessage(const SystemMessage& message);
};