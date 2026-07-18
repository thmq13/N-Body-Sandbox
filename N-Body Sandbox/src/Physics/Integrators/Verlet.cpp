#include <Physics/Integrators/Verlet.hpp>

#include <variant>

#include <Particle/Particle.hpp>

namespace NBody::Physics {
    Verlet::Verlet(const Parameters& parameters) noexcept : m_parameters(parameters) {} 

    void Verlet::step(Particle::ParticleSystem& buffer) {

    }

    void Verlet::setParameters(const std::vector<Core::ParameterSchema>& schemas) {
        for (const auto& schema : schemas) {
            if (schema.label == "Delta Time") {
                m_parameters.deltaTime = std::get<double>(schema.value);
            }
        }
    }
}

