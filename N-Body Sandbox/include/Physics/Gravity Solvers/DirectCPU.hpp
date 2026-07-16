#pragma once

#include <string>

#include <Physics/IGravitySolver.hpp>
#include <Core/Message.hpp>
#include <Helpers/Constants.hpp>

class DirectCPU : public IGravitySolver {
public:
    struct Parameters {
        double gravityConstant{ 1.0 };
        double softening{ 0.5 };
        bool useOMP{ false };
        bool useSIMD{ false };
    };

    explicit DirectCPU(const Parameters& parameters = {}) noexcept;

    ~DirectCPU() override = default;

    void solveGravity(ParticleSystem& buffer) override;

    void setParameters(const std::vector<ParameterSchema>& schemas) override;

    [[nodiscard]] virtual std::vector<ParameterSchema> getSchemas() const {
        return {
            ParameterSchema{"Gravity Constant", m_parameters.gravityConstant, 0.0, Limit::doubleMax},
            ParameterSchema{"Softening", m_parameters.softening, 0.0, Limit::doubleMax},
            ParameterSchema{"Use OMP", m_parameters.useOMP},
            ParameterSchema{"Use SIMD", m_parameters.useSIMD}
        };
    };

private:
    Parameters m_parameters;
};