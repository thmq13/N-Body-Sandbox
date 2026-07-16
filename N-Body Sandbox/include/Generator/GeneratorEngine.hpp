#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <random>

#include <Generator/IGenerator.hpp>
#include <Core/Message.hpp>

class MessageBus;
class ParticleBuffer;

class GeneratorEngine {
public:
    explicit GeneratorEngine(
        MessageBus& messageBus,
        std::shared_ptr<ParticleBuffer> particleBuffer,
        std::uint64_t initialSeed = 42
    );
    ~GeneratorEngine() = default;   
private:
    MessageBus& m_messageBus;
    std::shared_ptr<ParticleBuffer> m_particleBuffer{};

    std::unordered_map <std::string, std::unique_ptr<IGenerator>> m_availableGenerators{};

    std::uint64_t m_seed{};
    std::mt19937_64 m_rng{};
   
    void registerGenerators();
    void handleMessage(const SystemMessage& message);
};