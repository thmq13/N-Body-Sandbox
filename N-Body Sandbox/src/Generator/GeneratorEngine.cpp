#include <Generator/GeneratorEngine.hpp>

#include <iostream>
#include <variant>
#include <cstddef>

#include <Core/MessageBus.hpp>
#include <Generator/IGenerator.hpp>
#include <Generator/Shapes/UniformSphere.hpp>
#include <Particle/Particle.hpp>
    
namespace NBody::Generator {
    GeneratorEngine::GeneratorEngine(
        Core::MessageBus& messageBus,
        std::uint64_t initialSeed
    )
        : m_messageBus(messageBus),
        m_seed(initialSeed),
        m_rng(initialSeed)
    {
        m_messageBus.subscribe<Core::CmdGeneratePreviewParticles>([this](const Core::SystemMessage& message) {
            handleMessage(message);
        });

        m_messageBus.subscribe<Core::CmdRequestSchemas>([this](const Core::SystemMessage& message) {
            handleMessage(message);
        });

        m_messageBus.subscribe<Core::CmdSetRandomizeSeed>([this](const Core::SystemMessage& message) {
            handleMessage(message);
        });

        registerGenerators();

        std::cout << "[Generator Engine] Generators are ready.\n";
    }

    Generator::GeneratorEngine::~GeneratorEngine() {}

    void GeneratorEngine::handleMessage(const Core::SystemMessage& message) {
        std::visit([this](const auto& actualMessage) {
            using T = std::decay_t<decltype(actualMessage)>;

            if constexpr (std::is_same_v<T, Core::CmdGeneratePreviewParticles>) {
                std::cout << "[Generator Engine] Generate preview Cmd intercepted.\n";

                struct Particle::ParticleSystem temporaryBuffer{};

                std::size_t totalParticlesNeeded{ 0 };

                for (const auto& shape : actualMessage.shapes) {
                    for (const auto& schema : shape.schemas) {
                        if (schema.label == "Particle Count") {
                            totalParticlesNeeded += std::get<std::size_t>(schema.value);
                        }
                    }
                }
                temporaryBuffer.reserve(totalParticlesNeeded);

                for (const auto& [shapeId, schemas] : actualMessage.shapes) {
                    if (!m_availableGenerators.contains(shapeId)) {
                        std::cout << "[Generator Engine] Unknown shape requested: " << shapeId << ".\n";
                        continue;
                    }
                    m_availableGenerators[shapeId]->setParameters(schemas);
                    m_availableGenerators[shapeId]->generateOntoBuffer(temporaryBuffer, m_rng);
                }
                m_messageBus.publish(Core::CmdSendParticles{ false, std::move(temporaryBuffer) });
            }

            else if constexpr (std::is_same_v<T, Core::CmdRequestSchemas>) {
                std::cout << "[Generator Engine] Schemas request intercepted.\n";

                for (const auto& [shapeId, generator] : m_availableGenerators) {
                    m_messageBus.publish(Core::CmdSendSchemas{
                        static_cast<std::uint32_t>(Core::Module::Generator),
                        static_cast<std::uint32_t>(Core::GeneratorSubModule::Shape),
                        shapeId, generator->getSchemas()
                    });
                }
            }

            else if constexpr (std::is_same_v<T, Core::CmdSetRandomizeSeed>) {
                std::cout << "[Generator Engine] Seed request intercepted.\n";

                m_seed = actualMessage.seed;
                m_rng.seed(m_seed);
            }

        }, message);
    }

    void GeneratorEngine::registerGenerators() {
        m_availableGenerators["Uniform Sphere"] = std::make_unique<UniformSphere>();
    }
}

