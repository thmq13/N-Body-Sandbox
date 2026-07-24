#pragma once

#include <cstdint>
#include <iostream>
#include <format>
#include <string>
#include <utility>
#include <syncstream>

namespace NBody::Core {

    enum class ApplicationError : std::uint32_t {
        Success = 0,
        ErrPhysicsEngineFailed,
        ErrGeneratorFailed,
        ErrRenderingEngineFailed,
        ErrUIManagerFailed,
        ErrAlreadyInitialized,
        ErrNotInitialized
    };

    enum class PhysicsError : std::uint32_t {
        Success = 0,
        ErrAlreadyInitialized,
        ErrParticleBufferNull,
        ErrSolverNotFound,
        ErrIntegratorNotFound,
        ErrWorkerThreadStartFailed,
        ErrNotInitialized
    };

    enum class GeneratorError : std::uint32_t {
        Success = 0,
        ErrAlreadyInitialized,
        ErrGeneratorNotFound,
        ErrNotInitialized    
    };

    enum class RenderingError : std::uint32_t {
        Success = 0,
        ErrAlreadyInitialized,
        RendererNotFound,
        ErrNotInitialized,
        ErrAssetLoadingFailed
    };

    constexpr std::string_view ToString(ApplicationError err) noexcept {
        switch (err) {
            case ApplicationError::Success:                 return "Success";
            case ApplicationError::ErrPhysicsEngineFailed:  return "Physics Engine Failed";
            case ApplicationError::ErrGeneratorFailed:      return "Generator Failed";
            case ApplicationError::ErrRenderingEngineFailed:return "Rendering Engine Failed";
            case ApplicationError::ErrUIManagerFailed:      return "UI Manager Failed";
            case ApplicationError::ErrAlreadyInitialized:   return "EAlready Initialized";
            case ApplicationError::ErrNotInitialized:       return "Not Initialized";
            default:                                        return "Unknown Error";
        }
    }

    constexpr std::string_view ToString(PhysicsError err) noexcept {
        switch (err) {
            case PhysicsError::Success:                      return "Success";
            case PhysicsError::ErrAlreadyInitialized:        return "Already Initialized";
            case PhysicsError::ErrParticleBufferNull:        return "Particle Buffer Null";
            case PhysicsError::ErrSolverNotFound:            return "Solver Not Found";
            case PhysicsError::ErrIntegratorNotFound:        return "Integrator Not Found";
            case PhysicsError::ErrWorkerThreadStartFailed:   return "Worker Thread Start Failed";
            default:                                         return "Unknown Error";
        }
    }

    constexpr std::string_view ToString(GeneratorError err) noexcept {
        switch (err) {
            case GeneratorError::Success:                  return "Success";
            case GeneratorError::ErrAlreadyInitialized:    return "Already Initialized";
            case GeneratorError::ErrGeneratorNotFound:     return "Generator Not Found";
            case GeneratorError::ErrNotInitialized:        return "Not Initialized";
            default:                                       return "Unknown Error";
        }
    }

    enum class LogLevel : std::uint8_t {
        Trace = 0,
        Info,
        Warn,
        Error,
        Fatal
    };

    namespace Logging {

        namespace Color {
            constexpr const char* Reset = "\033[0m";
            constexpr const char* Gray = "\033[90m";
            constexpr const char* Green = "\033[32m";
            constexpr const char* Yellow = "\033[33m";
            constexpr const char* Red = "\033[31m";
            constexpr const char* Magenta = "\033[35m";
        }

        template <typename... Args>
        void log(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
            std::string formattedMessage{ std::format(fmt, std::forward<Args>(args)...) };
            std::osyncstream syncOut(std::cout);

            switch (level) {
                case LogLevel::Trace: syncOut << Color::Gray    << "[TRACE] " << Color::Reset << formattedMessage << '\n'; break;
                case LogLevel::Info:  syncOut << Color::Green   << "[INFO] "  << Color::Reset << formattedMessage << '\n'; break;
                case LogLevel::Warn:  syncOut << Color::Yellow  << "[WARN] "  << Color::Reset << formattedMessage << '\n'; break;
                case LogLevel::Error: syncOut << Color::Red     << "[ERROR] " << Color::Reset << formattedMessage << '\n'; break;
                case LogLevel::Fatal: syncOut << Color::Magenta << "[FATAL] " << Color::Reset << formattedMessage << '\n'; break;
            }
        }
    }

    #ifdef NDEBUG
        #define NBODY_TRACE(...) (void)0
        #define NBODY_INFO(...)  (void)0
        #define NBODY_WARN(...)  ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Warn, __VA_ARGS__)
        #define NBODY_ERROR(...) ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Error, __VA_ARGS__)
        #define NBODY_FATAL(...) ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Fatal, __VA_ARGS__)
    #else
        #define NBODY_TRACE(...) ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Trace, __VA_ARGS__)
        #define NBODY_INFO(...)  ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Info, __VA_ARGS__)
        #define NBODY_WARN(...)  ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Warn, __VA_ARGS__)
        #define NBODY_ERROR(...) ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Error, __VA_ARGS__)
        #define NBODY_FATAL(...) ::NBody::Core::Logging::log(::NBody::Core::LogLevel::Fatal, __VA_ARGS__)
    #endif

} // namespace NBody::Core


template <>
struct std::formatter<NBody::Core::ApplicationError> : std::formatter<std::string_view> {
    auto format(NBody::Core::ApplicationError err, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(NBody::Core::ToString(err), ctx);
    }
};

template <>
struct std::formatter<NBody::Core::PhysicsError> : std::formatter<std::string_view> {
    auto format(NBody::Core::PhysicsError err, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(NBody::Core::ToString(err), ctx);
    }
};

template <>
struct std::formatter<NBody::Core::GeneratorError> : std::formatter<std::string_view> {
    auto format(NBody::Core::GeneratorError err, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(NBody::Core::ToString(err), ctx);
    }
};