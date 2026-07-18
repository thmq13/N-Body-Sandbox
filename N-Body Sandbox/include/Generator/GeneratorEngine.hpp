#pragma once

#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include <Core/Message.hpp>

namespace NBody::Core { class MessageBus; }

namespace NBody::Generator {
    class Generator;

    class GeneratorEngine {
    public:
        explicit GeneratorEngine(
            Core::MessageBus& messageBus,
            std::uint64_t initialSeed = 42
        );
        ~GeneratorEngine();

    private:
        Core::MessageBus& m_messageBus;
        void handleMessage(const Core::SystemMessage& message);

        std::uint64_t m_seed{};
        std::mt19937_64 m_rng{};
        std::unordered_map <std::string, std::unique_ptr<Generator>> m_availableGenerators{};
        void registerGenerators();
    };
}
