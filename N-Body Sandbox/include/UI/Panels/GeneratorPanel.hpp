#pragma once

#include <array>
#include <string>

#include <UI/IPanel.hpp>
#include <Core/Message.hpp>
#include <UI/UITypes.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::UI {
    class GeneratorPanel : public IPanel {
    public:
        explicit GeneratorPanel(UIStorage& storage);
        ~GeneratorPanel() override = default;

        void draw(Core::MessageBus& messageBus) override;
    private:
        void setUpWindowAndStyle();
        void cleanUp();
        void drawPreviewList(Core::MessageBus& messageBus);

        UIStoredSubModules& m_subStorage;
        UIActiveTarget m_activeTarget{};
        std::vector<Core::CmdGeneratePreviewParticles::Shape> m_activeShapes{};
    };
}
