#pragma once

#include <string>

#include <Physics/IGravitySolver.hpp>
#include <Core/Message.hpp>
#include <Helpers/Constants.hpp>

namespace NBody::Particle { struct ParticleSystem; }

namespace NBody::Physics {
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

        void solveGravity(Particle::ParticleSystem& buffer) override;

        void setParameters(const std::vector<Core::ParameterSchema>& schemas) override;

        [[nodiscard]] virtual std::vector<Core::ParameterSchema> getSchemas() const {
            return {
                Core::ParameterSchema{"Gravity Constant", m_parameters.gravityConstant, 0.0, Constant::Limit::doubleMax},
                Core::ParameterSchema{"Softening", m_parameters.softening, 0.0, Constant::Limit::doubleMax},
                Core::ParameterSchema{"Use OMP", m_parameters.useOMP},
                Core::ParameterSchema{"Use SIMD", m_parameters.useSIMD}
            };
        };

    private:
        Parameters m_parameters;
    };
}
