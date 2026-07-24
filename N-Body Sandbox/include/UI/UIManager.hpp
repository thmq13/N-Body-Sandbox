#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Core/ApplicationState.hpp>
#include <Core/Message.hpp>
#include <type_traits>
#include <UI/IPanel.hpp>
#include <UI/UITypes.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {

    class UIManager final {
    public:
        explicit UIManager(Core::MessageBus& messageBus);

        ~UIManager() = default;

        UIManager(const UIManager&) = delete;
        UIManager& operator=(const UIManager&) = delete;
        UIManager(UIManager&&) = delete;
        UIManager& operator=(UIManager&&) = delete;

        void Draw(Core::ApplicationState state);

    private:
        Core::MessageBus& m_messageBus;

        template<typename T, typename... Args>
        T* CreatePanel(Core::ApplicationState state, Args&&... args) {
            static_assert(std::is_base_of_v<IPanel, T>, "T must derive from IPanel");

            auto panel = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = panel.get();

            m_ownedPanels.push_back(std::move(panel));
            m_statePanels[state].push_back(ptr);

            return ptr;
        }

        UIStorage m_storage{};
        std::unordered_map < Core::ApplicationState, std::vector<IPanel*> > m_statePanels{};
        std::vector<std::unique_ptr<IPanel>> m_ownedPanels{};

        void RegisterPanels();
        void HandleMessage(const Core::SystemMessage& message);
    };

} // namespace NBody::UI
