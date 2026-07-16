#pragma once
    
#include <vector>

struct ParticleSystem;
struct ParameterSchema;

class IGravitySolver {
public:
    virtual ~IGravitySolver() = default;
    virtual void solveGravity(ParticleSystem& buffer) = 0;
    virtual void setParameters(const std::vector<ParameterSchema>& schemas) = 0;
    [[nodiscard]] virtual std::vector<ParameterSchema> getSchemas() const = 0;
};