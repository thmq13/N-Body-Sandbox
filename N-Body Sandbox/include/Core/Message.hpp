#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <type_traits>
#include <array>
#include <algorithm>

#include <imgui.h>
#include <raylib.h>

#include <Core/AppState.hpp>
#include <Helpers/Vec.hpp>
#include <Helpers/Constants.hpp>
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

    inline constexpr std::size_t moduleCount = static_cast<std::size_t>(Module::Count);

    inline const std::array<std::string, moduleCount> moduleNames{ "Physics", "Generator" };

    enum class PhysicsSubModule : std::uint32_t {
        GravitySolver,
        Integrator,
        Count
    };
    enum class GeneratorSubModule : std::uint32_t {
        Shape,
        Count
    };

    inline const std::array<std::size_t, moduleCount> subModuleCount{
        static_cast<std::size_t>(PhysicsSubModule::Count),
        static_cast<std::size_t>(GeneratorSubModule::Count)
    };

    inline const std::array<std::vector<std::string>, moduleCount> subModuleNames{
        std::vector<std::string>{ "Gravity Solver", "Integrator" },
        std::vector<std::string>{ "Shape" }
    };

    // --- LIST OF COMMANDS ---

    // System level
    struct CmdExitApplication {};

    struct CmdRequestStateChange { AppState requestedState; };


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
        std::vector<ParameterSchema> schemas;
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

