#pragma once

#include <vector>

namespace NBody::Particle { struct ParticleSystem; }
namespace NBody::Core { struct ParameterSchema; }

namespace NBody::Physics {
    class Integrator {
    public:
        virtual ~Integrator() = default;
        virtual void step(Particle::ParticleSystem& particleSystem) = 0;
        virtual void setParameter(const Core::ParameterSchema& schema) = 0;
        [[nodiscard]] virtual std::vector<Core::ParameterSchema> getSchemas() const = 0;
    };
}
