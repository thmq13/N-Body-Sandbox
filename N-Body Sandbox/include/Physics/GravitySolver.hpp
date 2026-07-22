#pragma once
    
#include <vector>

namespace NBody::Particle { struct ParticleSystem; }
namespace NBody::Core { struct ParameterSchema; }

namespace NBody::Physics {
    class GravitySolver {
    public:
        virtual ~GravitySolver() = default;
        virtual void solveGravity(Particle::ParticleSystem& particleSystem) = 0;
        virtual void setParameter(const Core::ParameterSchema& schema) = 0;
        [[nodiscard]] virtual std::vector<Core::ParameterSchema> getSchemas() const = 0;
    };
}
