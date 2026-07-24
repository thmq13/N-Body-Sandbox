#include <Physics/Integrators/Verlet.hpp>

#include <variant>
#include <vector>

#include <Core/Message.hpp>
#include <Particle/Particle.hpp>

namespace NBody::Physics {
    Verlet::Verlet(const Parameters& parameters) noexcept : m_parameters(parameters) {} 

    void Verlet::Step(Particle::ParticleSystem& particleSystem) {

    }

    void Verlet::SetParameter(const Core::ParameterSchema& schema) {
        if (schema.label == "Delta Time") {
            m_parameters.deltaTime = std::get<double>(schema.value);
        }
    }
}

