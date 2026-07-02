#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <Core/Events.hpp>
#include <UI/UIState.hpp>
#include <UI/IPanel.hpp>

class MessageBus;

class UIManager {
public:
    explicit UIManager(MessageBus& messageBus);

    ~UIManager() = default;

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    UIManager(UIManager&&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    void update();

    [[nodiscard]] UIState getState() const noexcept { return m_UIState; }

private:
    UIState m_UIState{ UIState::TitleScreen };
    
    std::unordered_map < UIState, std::vector<IPanel*> > m_statePanels{};
    std::vector<std::unique_ptr<IPanel>> m_ownedPanels{};

    MessageBus& m_messageBus;

    void registerPanels();

    template<typename T, typename... Args>
    T* createPanel(UIState state, Args&&... args) {
        static_assert(std::is_base_of_v<IPanel, T>, "T must derive from IPanel");

        auto panel = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = panel.get();

        m_ownedPanels.push_back(std::move(panel));
        m_statePanels[state].push_back(ptr);

        return ptr;
    }

    void handleMessage(const SystemMessage& message);
};