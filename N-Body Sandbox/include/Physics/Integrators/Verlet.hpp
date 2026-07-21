#pragma once

#include <vector>

#include <Core/Message.hpp>
#include <Helpers/Constants.hpp>
#include <Physics/Integrator.hpp>

namespace NBody::Particle { struct ParticleSystem;  }

namespace NBody::Physics {
    class Verlet : public Integrator {
    public:
        struct Parameters {
            double deltaTime{ 0.01 };
        };

        explicit Verlet(const Parameters& parameters = {}) noexcept;

        ~Verlet() override = default;

        void step(Particle::ParticleSystem& particleSystem) override;

        void setParameters(const std::vector<Core::ParameterSchema>& schemas) override;

        [[nodiscard]] std::vector<Core::ParameterSchema> getSchemas() const override {
            return {
                Core::ParameterSchema{"Delta Time", m_parameters.deltaTime, Constant::Limit::doubleEpsilon, Constant::Limit::doubleMax}
            };
        }

    private:
        Parameters m_parameters;
    };
}
