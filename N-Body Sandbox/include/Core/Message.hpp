#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <variant>
#include <vector>

#include <raylib.h>

#include <Core/ApplicationState.hpp>
#include <Helpers/Vec.hpp>
#include <Particle/Particle.hpp>

// --- GENERAL TYPES FOR COMMUNICATION ---

namespace NBody::Core {

    // Parameter standard
    using ParameterValue = std::variant<
        bool,
        int, std::size_t,
        float, double,
        std::string,
        Math::Vec3,
        Color
    >;

    struct ParameterSchema {
        std::string label{};
        ParameterValue value{};
        ParameterValue minValue{};
        ParameterValue maxValue{};
    };

    // Moules & submodules
    enum class Module : std::uint32_t {
        Physics,
        Generator,
        Count
    };

    enum class PhysicsSubModule : std::uint32_t {
        GravitySolver,
        Integrator,
        Count
    };

    enum class GeneratorSubModule : std::uint32_t {
        Shape,
        Count
    };

    inline constexpr std::size_t moduleCount = static_cast<std::size_t>(Module::Count);
    inline constexpr std::size_t physicsSubModuleCount = static_cast<std::size_t>(PhysicsSubModule::Count);
    inline constexpr std::size_t generatorSubModuleCount = static_cast<std::size_t>(GeneratorSubModule::Count);

    inline constexpr std::array<const char*, moduleCount> moduleName{ "Physics", "Generator" };
    inline constexpr std::array<const char*, physicsSubModuleCount> physicsSubModuleName{ "Gravity Solver", "Integrator"};
    inline constexpr std::array<const char*, generatorSubModuleCount> generatorSubModuleName{ "Shape" };

    inline constexpr std::array<std::size_t, moduleCount> subModuleCount{ physicsSubModuleCount, generatorSubModuleCount };

    inline constexpr std::array<std::span<const char * const>, moduleCount> subModuleName{
        physicsSubModuleName,
        generatorSubModuleName
    };

    // --- LIST OF COMMANDS ---

    // System level
    struct CmdExitApplication {};

    struct CmdRequestStateChange { ApplicationState requestedState; };


    // UI communication
    struct CmdRequestSchemas {};

    struct CmdSendSchemas {
        std::uint32_t moduleId;
        std::uint32_t subModuleId;
        std::string senderId;
        std::vector<ParameterSchema> schemas;
    };

    struct CmdSetActiveOption {
        std::uint32_t moduleId;
        std::uint32_t subModuleId;
        std::string activeId;
    };

    struct CmdSetConfig {
        std::uint32_t moduleId;
        std::uint32_t subModuleId;
        std::string targetId;
        ParameterSchema schema;
    };

    // Generator
    struct CmdGeneratePreviewParticles {
        struct Shape {
            std::string shapeId;
            std::vector<ParameterSchema> schemas;
        };
        std::vector<Shape> shapes;
    };

    struct CmdSendParticles {
        bool shouldAppend;
        Particle::ParticleSystem particleSystem;
    };

    struct CmdSetRandomizeSeed { std::uint64_t seed; };


    // --- THE FINAL VARIANT ---
    using SystemMessage = std::variant <
        CmdExitApplication,
        CmdRequestStateChange,

        CmdRequestSchemas,
        CmdSendSchemas,
        CmdSetActiveOption,
        CmdSetConfig,

        CmdGeneratePreviewParticles,
        CmdSendParticles,
        CmdSetRandomizeSeed
    >;
}

