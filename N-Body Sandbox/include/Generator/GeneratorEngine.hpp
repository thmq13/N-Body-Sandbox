#pragma once

#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <expected>
#include <vector>

#include <Core/MessageBus.hpp>
#include <Core/Message.hpp>
#include <Core/Logger.hpp>

namespace NBody::Generator {

    class Generator;

    class GeneratorEngine final : public Core::MessageSubscriber<GeneratorEngine> {
    public:
        friend class Core::MessageSubscriber<GeneratorEngine>;

        using Result = std::expected<void, Core::GeneratorError>;

        explicit GeneratorEngine(Core::MessageBus& messageBus, std::uint64_t initialSeed = 42) noexcept;
        ~GeneratorEngine();

        GeneratorEngine(const GeneratorEngine&) = delete;
        GeneratorEngine& operator=(const GeneratorEngine&) = delete;
        GeneratorEngine(GeneratorEngine&&) noexcept = delete;
        GeneratorEngine& operator=(GeneratorEngine&&) noexcept = delete;

        [[nodiscard]] Result InitializeEngine();
        void SetSeed(std::uint64_t seed) noexcept;

    private:
        bool m_isInitialized{ false };
        
        Core::MessageBus& m_messageBus;
        void HandleMessage(const Core::SystemMessage& message);
        template <typename MessageType>
        void SubscribeToMessage() {
            m_messageBus.template Subscribe<MessageType>([this](const Core::SystemMessage& message) {
                HandleMessage(message);
            });
        }

        std::uint64_t   m_seed{};
        std::mt19937_64 m_rng{};

        std::unordered_map <std::string, std::unique_ptr<Generator>> m_generators{};
        void RegisterGenerators();
        void SendSchemas();
        [[nodiscard]] Result GenerateAndSendParticles(const std::vector<Core::CmdGeneratePreviewParticles::Shape>& shapes);
    };

} // namespace NBody::Generator
