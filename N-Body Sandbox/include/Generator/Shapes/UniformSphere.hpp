#pragma once

#include <cstddef>

#include <raylib.h>

#include <Generator/IGenerator.hpp>
#include <Helpers/Vec.hpp>
#include <Helpers/Constants.hpp>
#include <Core/Message.hpp>

struct ParticleSystem;

class UniformSphere : public IGenerator {
public:
    struct Parameters {
        std::size_t particleCount{ 100 };
        double totalMass{ 1000.0 };
        double radius{ 10.0 };
        float particleRadius{ 10.0 };
        Color color{ WHITE };
        Vec3 origin{ 0.0, 0.0, 0.0 };
    };

    explicit UniformSphere(const Parameters& parameters = Parameters{}) noexcept;

    ~UniformSphere() override = default;

    void generateOntoBuffer(ParticleSystem& buffer, std::mt19937_64& rng) override;

    void setParameters(const std::vector<ParameterSchema>& schemas) override;

    [[nodiscard]] std::vector<ParameterSchema> getSchemas() const override {
        return {
            ParameterSchema{"Particle Count", m_parameters.particleCount, 0.0, Limit::sizeTMax},
            ParameterSchema{"Total Mass",     m_parameters.totalMass,     0.0, Limit::doubleMax},
            ParameterSchema{"Radius",         m_parameters.radius,        0.0, Limit::doubleMax},
            ParameterSchema{"Particle Radius",m_parameters.particleRadius,0.0, Limit::doubleMax},
            ParameterSchema{"Color",          m_parameters.color                             },
            ParameterSchema{"Origin",         m_parameters.origin}
        };
    }

private:    
    Parameters m_parameters{};
};