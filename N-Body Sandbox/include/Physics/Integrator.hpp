#pragma once

#include <vector>

namespace NBody::Particle { struct ParticleSystem; }
namespace NBody::Core { struct ParameterSchema; }

namespace NBody::Physics {
    class Integrator {
    public:
        virtual ~Integrator() = default;
        virtual void step(Particle::ParticleSystem& buffer) = 0;
        virtual void setParameters(const std::vector<Core::ParameterSchema>& schemas) = 0;
        [[nodiscard]] virtual std::vector<Core::ParameterSchema> getSchemas() const = 0;
    };
}
