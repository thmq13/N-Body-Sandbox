#pragma once

#include <cstddef>
#include <random>
#include <vector>

#include <raylib.h>

#include <Core/Message.hpp>
#include <Generator/Generator.hpp>
#include <Helpers/Constants.hpp>
#include <Helpers/Vec.hpp>

namespace NBody::Particle { struct ParticleSystem; }

namespace NBody::Generator {
    class UniformSphere : public Generator {
    public:
        struct Parameters {
            std::size_t particleCount{ 100 };
            double totalMass{ 1000.0 };
            double radius{ 10.0 };
            float particleRadius{ 10.0 };
            Color color{ WHITE };
            Math::Vec3 origin{ 0.0, 0.0, 0.0 };
        };

        explicit UniformSphere(const Parameters& parameters = Parameters{}) noexcept;

        ~UniformSphere() override = default;

        void generateOntoBuffer(Particle::ParticleSystem& buffer, std::mt19937_64& rng) override;

        void setParameters(const std::vector<Core::ParameterSchema>& schemas) override;

        [[nodiscard]] std::vector<Core::ParameterSchema> getSchemas() const override {
            return {
                Core::ParameterSchema{"Particle Count", m_parameters.particleCount, 1,   Constant::Limit::sizeTMax},
                Core::ParameterSchema{"Total Mass",     m_parameters.totalMass,     0.0, Constant::Limit::doubleMax},
                Core::ParameterSchema{"Radius",         m_parameters.radius,        0.0, Constant::Limit::doubleMax},
                Core::ParameterSchema{"Particle Radius",m_parameters.particleRadius,0.0, Constant::Limit::doubleMax},
                Core::ParameterSchema{"Color",          m_parameters.color                             },
                Core::ParameterSchema{"Origin",         m_parameters.origin}
            };
        }

    private:
        Parameters m_parameters{};
    };
}
