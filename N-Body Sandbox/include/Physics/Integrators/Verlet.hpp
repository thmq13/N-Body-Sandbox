#pragma once

#include <vector>

#include <Core/Message.hpp>
#include <Helpers/Constants.hpp>
#include <Physics/Integrator.hpp>

namespace NBody::Particle { struct ParticleSystem;  }

namespace NBody::Physics {
    class Verlet final : public Integrator {
    public:
        struct Parameters {
            double deltaTime{ 0.01 };
        };

        explicit Verlet(const Parameters& parameters = {}) noexcept;

        ~Verlet() override = default;

        void Step(Particle::ParticleSystem& particleSystem) override;

        void SetParameter(const Core::ParameterSchema& schema) override;

        [[nodiscard]] std::vector<Core::ParameterSchema> GetSchemas() const override {
            return {
                Core::ParameterSchema{"Delta Time", m_parameters.deltaTime, Constant::Limit::doubleEpsilon, Constant::Limit::doubleMax}
            };
        }

    private:
        Parameters m_parameters;
    };

} // namespace NBody::Physics
