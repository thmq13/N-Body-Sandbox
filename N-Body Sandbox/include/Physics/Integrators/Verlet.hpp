#pragma once

#include <Physics/IIntegrator.hpp>
#include <Helpers/Constants.hpp>
#include <Core/Message.hpp>

class Verlet : public IIntegrator {
public:
    struct Parameters {
        double deltaTime{0.01};
    };

    explicit Verlet(const Parameters& parameters = {}) noexcept;

    ~Verlet() override = default;

    void step(ParticleSystem& buffer) override;
    
    void setParameters(const std::vector<ParameterSchema>& schemas) override;

    [[nodiscard]] std::vector<ParameterSchema> getSchemas() const override {
        return {
            ParameterSchema{"Delta Time", m_parameters.deltaTime, Limit::doubleEpsilon, Limit::doubleMax}
        };
    }

private:
    Parameters m_parameters;
};