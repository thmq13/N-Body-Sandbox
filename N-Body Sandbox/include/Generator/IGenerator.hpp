#pragma once

#include <random>
#include <vector>

struct ParticleSystem;
struct ParameterSchema;

class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual void generateOntoBuffer(ParticleSystem& buffer, std::mt19937_64& rng) = 0;
    virtual void setParameters(const std::vector<ParameterSchema>& schemas) = 0;
    [[nodiscard]] virtual std::vector<ParameterSchema> getSchemas() const = 0;
};