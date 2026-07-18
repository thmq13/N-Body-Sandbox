#pragma once

#include <random>
#include <vector>

namespace NBody::Particle { struct ParticleSystem; };
namespace NBody::Core { struct ParameterSchema; }

namespace NBody::Generator {
    class Generator {
    public:
        virtual ~Generator() = default;
        virtual void generateOntoBuffer(Particle::ParticleSystem& buffer, std::mt19937_64& rng) = 0;
        virtual void setParameters(const std::vector<Core::ParameterSchema>& schemas) = 0;
        [[nodiscard]] virtual std::vector<Core::ParameterSchema> getSchemas() const = 0;
    };
}