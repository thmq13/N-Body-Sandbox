#pragma once

#include <vector>
#include <variant>
#include <cstddef>
#include <cstdint>

struct ComputeConfig {
    enum class Algorithm { DirectN2, BarnesHut };
    enum class Hardware { CPU, GPU };

    Algorithm algorithm{ Algorithm::DirectN2 };
    Hardware hardware{ Hardware::CPU };
    bool useOpenMP = true;
    bool useSIMD = true;
};

struct PhysicsConfig {
    enum class Integrator { Euler, Verlet, RK4 };

    double GConstant{ 1.0 };
    double deltaTime{ 1.0 / 60.0 };
    double softeningLength{ 0.5 };
    double theta = 0.5;

    Integrator integrator{ Integrator::Verlet };
};


struct SpawnerConfig {
    enum class Shape { UniformSphere, Plummer, DiskGalaxy };

    struct UniformSphereParams {
        double radius{ 10.0 };
        double totalMass{ 1000.0 };
        std::size_t bodyCount{ 100 };
    };
    struct PlummerParams {
        double scaleRadius{ 5.0 };
        double totalMass{ 1000.0 };
        std::size_t bodyCount{ 200 };
    };
    struct DiskGalaxyParams {
        double radius{ 15.0 };
        double thickness{ 0.5 };
        double totalMass{ 2000.0 };
        std::size_t bodyCount{ 300 };
    };
    using ShapeParams = std::variant<UniformSphereParams, PlummerParams, DiskGalaxyParams>;

    struct ClusterConfig {
        std::uint32_t id;
        ShapeParams params;
    };

    std::vector<ClusterConfig> clusters;
};