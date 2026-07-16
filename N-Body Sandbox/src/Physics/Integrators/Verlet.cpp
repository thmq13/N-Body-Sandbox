#include <Physics/Integrators/Verlet.hpp>

Verlet::Verlet(const Parameters& parameters) noexcept : m_parameters(parameters)
{
}

void Verlet::step(ParticleSystem& buffer)
{
}

void Verlet::setParameters(const std::vector<ParameterSchema>& schemas) {
    for (const auto& schema : schemas) {
        if (schema.label == "Delta Time") {
            m_parameters.deltaTime = std::get<double>(schema.value);
        }
    }
}
