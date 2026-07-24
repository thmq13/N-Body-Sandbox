#pragma once

#include <atomic>
#include <mutex>

#include <Particle/Particle.hpp>

namespace NBody::Particle {

    class ParticleBuffer final {
    public:
        ParticleBuffer() = default;
        ~ParticleBuffer() = default;

        ParticleBuffer(const ParticleBuffer&) = delete;
        ParticleBuffer& operator=(const ParticleBuffer&) = delete;
        ParticleBuffer(ParticleBuffer&&) = delete;
        ParticleBuffer& operator=(ParticleBuffer&&) = delete;

        void CommitBackBuffer();
        void UpdateFrontBuffer();

        [[nodiscard]] ParticleSystem& GetBackBuffer() noexcept {
            return m_backBuffer;
        }

        [[nodiscard]] const ParticleSystem& GetFrontBuffer() const noexcept {
            return m_frontBuffer;
        }

    private:
        ParticleSystem m_frontBuffer{};
        ParticleSystem m_backBuffer{};
        ParticleSystem m_sharedBuffer{};

        std::atomic<bool> m_newFrameReady{ false };
        mutable std::mutex m_mutex;
    };

} // namespace NBody::Particle
