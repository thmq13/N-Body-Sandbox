#include <Physics/Gravity Solvers/DirectCPU.hpp>

#include <cstddef>
#include <variant>
#include <vector>

#include <Core/Message.hpp>
#include <Particle/Particle.hpp>

namespace NBody::Physics {

    DirectCPU::DirectCPU(const Parameters& parameters) noexcept : m_parameters(parameters) {}

    void DirectCPU::SolveGravity(Particle::ParticleSystem& particleSystem) {
        const std::size_t particleCount{ particleSystem.GetSize() };
        if (particleCount <= 1) { return;  }

        const double softening_squared = m_parameters.softening * m_parameters.softening;
    }

    void DirectCPU::SetParameter(const Core::ParameterSchema& schema) {
        if (schema.label == "Gravity Constant") {
            m_parameters.gravityConstant = std::get<double>(schema.value);
        }
        else if (schema.label == "Softening") {
            m_parameters.softening = std::get<double>(schema.value);
        }
        else if (schema.label == "Use OMP") {
            m_parameters.useOMP = std::get<bool>(schema.value);
        }
        else if (schema.label == "Use SIMD") {
            m_parameters.useSIMD = std::get<bool>(schema.value);
        }
    }

} // namespace NBody::Physics

