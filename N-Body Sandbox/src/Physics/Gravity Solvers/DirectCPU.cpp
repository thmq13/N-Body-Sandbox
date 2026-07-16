#include <Physics/Gravity Solvers/DirectCPU.hpp>

#include <cstddef>
#include <cmath>

#include <Particle/Particle.hpp>

DirectCPU::DirectCPU(const Parameters& parameters) noexcept : m_parameters(parameters) {}

void DirectCPU::solveGravity(ParticleSystem& buffer) {
    const std::size_t particleCount{ buffer.getSize() };
    if (particleCount <= 1) {
        return;
    }

    const double softening_squared = m_parameters.softening * m_parameters.softening;

    
}

void DirectCPU::setParameters(const std::vector<ParameterSchema>& schemas) {
    for (const auto& schema : schemas) {
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
}
