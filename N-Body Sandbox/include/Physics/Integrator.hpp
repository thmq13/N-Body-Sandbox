#pragma once

#include <vector>

namespace NBody::Particle { struct ParticleSystem; }
namespace NBody::Core { struct ParameterSchema; }

namespace NBody::Physics {

    class Integrator {
    public:
        virtual ~Integrator() = default;
        virtual void Step(Particle::ParticleSystem& particleSystem) = 0;
        virtual void SetParameter(const Core::ParameterSchema& schema) = 0;
        [[nodiscard]] virtual std::vector<Core::ParameterSchema> GetSchemas() const = 0;
    };

} // namespace NBody::Physics
