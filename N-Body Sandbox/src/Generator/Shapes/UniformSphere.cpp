#include <Generator/Shapes/UniformSphere.hpp>

#include <variant>

#include <Particle/Particle.hpp>

UniformSphere::UniformSphere(const Parameters& parameters) noexcept : m_parameters(parameters) {}

void UniformSphere::generateOntoBuffer(ParticleSystem& buffer, std::mt19937_64& rng) {
    std::size_t startIdx = buffer.prepareAppend(m_parameters.particleCount);
    std::size_t count = m_parameters.particleCount;

    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for (std::size_t i = 0; i < count; ++i) {
        std::size_t targetIdx = startIdx + i;
        double x{}, y{}, z{}, r2{};
        do {
            x = dist(rng);
            y = dist(rng);
            z = dist(rng);
            r2 = x * x + y * y + z * z;
        } while (r2 > 1.0);

        Vec3 localPosition{ x, y, z };
        Vec3 finalPosition = localPosition * m_parameters.radius + m_parameters.origin;
        buffer.positionX[targetIdx] = finalPosition.x;
        buffer.positionY[targetIdx] = finalPosition.y;
        buffer.positionZ[targetIdx] = finalPosition.z;
        
        buffer.radius[targetIdx] = m_parameters.particleRadius;
        buffer.color[targetIdx] = m_parameters.color;
    }
}

void UniformSphere::setParameters(const std::vector<ParameterSchema>& schemas) {
    for (const auto& schema : schemas) {
        if (schema.label == "Particle Count") {
            m_parameters.particleCount = std::get<std::size_t>(schema.value);
        } 
        else if (schema.label == "Total Mass") {
            m_parameters.totalMass = std::get<double>(schema.value);
        }
        else if (schema.label == "Radius") {
            m_parameters.radius = std::get<double>(schema.value);
        }
        else if (schema.label == "Particle Radius") {
            m_parameters.particleRadius = std::get<float>(schema.value);
        }
        else if (schema.label == "Color") {
            m_parameters.color = std::get<Color>(schema.value);
        }
        else if (schema.label == "Origin") {
            m_parameters.origin = std::get<Vec3>(schema.value);
        }
    }
}

