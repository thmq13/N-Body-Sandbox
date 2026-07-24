#include <Generator/GeneratorEngine.hpp>

#include <Core/MessageBus.hpp>
#include <Generator/Generator.hpp>
#include <Generator/Shapes/UniformSphere.hpp>
#include <Particle/Particle.hpp>
#include <Core/Message.hpp>
#include <Core/Logger.hpp>    

import std;

namespace NBody::Generator {
    GeneratorEngine::GeneratorEngine(
        Core::MessageBus& messageBus,
        std::uint64_t initialSeed
    ) noexcept 
        : m_messageBus(messageBus),
        m_seed(initialSeed),
        m_rng(initialSeed)  {}

    GeneratorEngine::~GeneratorEngine() {
        NBODY_INFO("GENERATOR ENGINE: Deallocating resources.");
    }

    GeneratorEngine::Result GeneratorEngine::InitializeEngine() {
        if (m_isInitialized) {
            return std::unexpected(Core::GeneratorError::ErrAlreadyInitialized);
        }

        SubscribeToMessages<
            Core::CmdGeneratePreviewParticles,
            Core::CmdRequestSchemas,
            Core::CmdSetRandomizeSeed
        >();

        RegisterGenerators();

        m_isInitialized = true;

        NBODY_INFO("GENERATOR ENGINE: Engine initialized successfully.");
        return{};
    }

    void GeneratorEngine::SetSeed(std::uint64_t seed) noexcept { 
        NBODY_TRACE("GENERATOR ENGINE: Set seed to {}.", seed);
        m_seed = seed; 
        m_rng.seed(seed); 
    }

    void GeneratorEngine::HandleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& msg) {
            using MessageType = std::decay_t<decltype(msg)>;

            if constexpr (std::is_same_v<MessageType, Core::CmdGeneratePreviewParticles>) {
                auto genRes{ GenerateAndSendParticles(msg.shapes) };
                if (!genRes) {
                    NBODY_ERROR("GENERATOR ENGINE: Executing CmdGeneratePreviewParticles failed: ", genRes.error());
                }
            }
            else if constexpr (std::is_same_v<MessageType, Core::CmdRequestSchemas>) {
                SendSchemas();
            }
            else if constexpr (std::is_same_v<MessageType, Core::CmdSetRandomizeSeed>) {
                SetSeed(msg.seed);
            }

        }, message);
    }

    void GeneratorEngine::RegisterGenerators() {
        m_generators["Uniform Sphere"] = std::make_unique<UniformSphere>();
    }

    void GeneratorEngine::SendSchemas() {
        for (const auto& [shapeId, generator] : m_generators) {
            m_messageBus.Publish(Core::CmdSendSchemas{
                static_cast<std::uint32_t>(Core::Module::Generator),
                static_cast<std::uint32_t>(Core::GeneratorSubModule::Shape),
                shapeId, generator->GetSchemas()
            });
        }
        NBODY_INFO("GENERATOR ENGINE: Published schemas of components.");
    }

    GeneratorEngine::Result GeneratorEngine::GenerateAndSendParticles(const std::vector<Core::CmdGeneratePreviewParticles::Shape>& shapes) {
        if (!m_isInitialized) {
            return std::unexpected(Core::GeneratorError::ErrNotInitialized);
        }

        std::size_t totalParticlesNeeded{ 0 };
        for (const auto& shape : shapes) {
            if (!m_generators.contains(shape.shapeId)) {
                NBODY_ERROR("GENERATOR ENGINE: Unknown shape requested: {}", shape.shapeId);
                return std::unexpected(Core::GeneratorError::ErrGeneratorNotFound);
            }
            for (const auto& schema : shape.schemas) {
                if (schema.label == "Particle Count") {
                    totalParticlesNeeded += std::get<std::size_t>(schema.value);
                }
            }
        }

        NBODY_TRACE("GENERATOR ENGINE: Generating {} particle(s) and publishing.", totalParticlesNeeded);

        struct Particle::ParticleSystem temporaryBuffer {};
        temporaryBuffer.Reserve(totalParticlesNeeded);

        for (const auto& [shapeId, schemas] : shapes) {
            auto& generator = m_generators.at(shapeId);
            generator->SetParameters(schemas);
            generator->GenerateOntoBuffer(temporaryBuffer, m_rng);
        }
        m_messageBus.Publish(Core::CmdSendParticles{ false, std::move(temporaryBuffer) });

        return {};
    }
}

