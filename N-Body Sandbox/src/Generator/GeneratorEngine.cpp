#include <Generator/GeneratorEngine.hpp>

#include <iostream>

#include <Core/MessageBus.hpp>
#include <Particle/ParticleBuffer.hpp>
#include <Generator/IGenerator.hpp>
#include <Generator/Shapes/UniformSphere.hpp>
    
GeneratorEngine::GeneratorEngine(
    MessageBus& messageBus, 
    std::shared_ptr<ParticleBuffer> particleBuffer,
    std::uint64_t initialSeed
)
    : m_messageBus(messageBus),
      m_particleBuffer(std::move(particleBuffer)),
      m_seed(initialSeed),
      m_rng(initialSeed)
{
    m_messageBus.subscribe<CmdGeneratePreviewParticles>([this](const SystemMessage& message) {
        handleMessage(message);
    });

    m_messageBus.subscribe<CmdRequestSchemas>([this](const SystemMessage& message) {
        handleMessage(message);
    });

    m_messageBus.subscribe<CmdSetRandomizeSeed>([this](const SystemMessage& message) {
        handleMessage(message);
    });

    registerGenerators();

    std::cout << "[Generator Engine] Generators are ready.\n";
} 


void GeneratorEngine::registerGenerators() {
    m_availableGenerators["Uniform Sphere"] = std::make_unique<UniformSphere>();
}

void GeneratorEngine::handleMessage(const SystemMessage& message) {
    std::visit([this](const auto& actualMessage) {
        using T = std::decay_t<decltype(actualMessage)>;

        if constexpr (std::is_same_v<T, CmdGeneratePreviewParticles>) {
            std::cout << "[Generator Engine] Generate preview Cmd intercepted.\n";

            auto& backBuffer = m_particleBuffer->getBackBuffer();
            for (const auto& [shapeId, schemas] : actualMessage.shapes) {
                if (!m_availableGenerators.contains(shapeId)) {
                    std::cout << "[Generator Engine] Unknown shape requested: " << shapeId << ".\n";
                    continue;
                }
                m_availableGenerators[shapeId]->setParameters(schemas);
                m_availableGenerators[shapeId]->generateOntoBuffer(backBuffer, m_rng);
            }
            m_particleBuffer->commitBackBuffer();
        }

        else if constexpr (std::is_same_v<T, CmdRequestSchemas>) {
            std::cout << "[Generator Engine] Schemas request intercepted.\n";

            for (const auto& [shapeId, generator] : m_availableGenerators) {
                m_messageBus.publish(CmdSendSchemas{
                    static_cast<std::uint32_t>(Module::Generator), 
                    static_cast<std::uint32_t>(GeneratorSubModule::Shape),
                    shapeId, generator->getSchemas()
                });
            }
        }

        else if constexpr (std::is_same_v<T, CmdSetRandomizeSeed>) {
            std::cout << "[Generator Engine] Seed request intercepted.\n";

            m_seed = actualMessage.seed;
            m_rng.seed(m_seed);
        }

    }, message);
}
