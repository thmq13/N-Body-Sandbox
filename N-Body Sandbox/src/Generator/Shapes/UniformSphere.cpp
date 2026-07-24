#include <Generator/Shapes/UniformSphere.hpp>

#include <cassert>
#include <cstddef>
#include <random>
#include <variant>
#include <vector>

#include <raylib.h>

#include <Core/Message.hpp>
#include <Helpers/Vec.hpp>
#include <Particle/Particle.hpp>

namespace NBody::Generator {

    UniformSphere::UniformSphere(const Parameters& parameters) noexcept : m_parameters(parameters) {}

    void UniformSphere::GenerateOntoBuffer(Particle::ParticleSystem& buffer, std::mt19937_64& rng) {
        std::size_t startIdx{ buffer.EnlargeSize(m_parameters.particleCount) };
        std::size_t particleCount{ m_parameters.particleCount };
        
        assert(m_parameters.particleCount > 0);
        double particleMass{ m_parameters.totalMass / particleCount };

        std::uniform_real_distribution<double> dist(-1.0, 1.0);

        for (std::size_t i = 0; i < particleCount; ++i) {

            std::size_t targetIdx = startIdx + i;
            double x{}, y{}, z{}, r2{};
            do {
                x = dist(rng);
                y = dist(rng);
                z = dist(rng);
                r2 = x * x + y * y + z * z;
            } while (r2 > 1.0);

            Math::Vec3 localPosition{ x, y, z };
            Math::Vec3 finalPosition = localPosition * m_parameters.radius + m_parameters.origin;
            buffer.positionX[targetIdx] = finalPosition.x;
            buffer.positionY[targetIdx] = finalPosition.y;
            buffer.positionZ[targetIdx] = finalPosition.z;

            buffer.mass[targetIdx] = particleMass;
            buffer.radius[targetIdx] = m_parameters.particleRadius;
            buffer.color[targetIdx] = m_parameters.color;
        }
    }

    void UniformSphere::SetParameters(const std::vector<Core::ParameterSchema>& schemas) {
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
                m_parameters.origin = std::get<Math::Vec3>(schema.value);
            }
        }
    }

} // namespace NBody::Generator


