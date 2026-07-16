#pragma once

#include <atomic>
#include <mutex>

#include <Particle/Particle.hpp>

class ParticleBuffer {
public:
    ParticleBuffer() = default;
    ~ParticleBuffer() = default;

    ParticleBuffer(const ParticleBuffer&) = delete;
    ParticleBuffer& operator=(const ParticleBuffer&) = delete;
    ParticleBuffer(ParticleBuffer&&) = delete;
    ParticleBuffer& operator=(ParticleBuffer&&) = delete;

    void commitBackBuffer();
    void updateFrontBuffer();

    [[nodiscard]] ParticleSystem& getBackBuffer() noexcept {
        return m_backBuffer;
    }

    [[nodiscard]] const ParticleSystem& getFrontBuffer() const noexcept {
        return m_frontBuffer;
    }

private:
    ParticleSystem m_frontBuffer{};
    ParticleSystem m_backBuffer{};
    ParticleSystem m_sharedBuffer{};

    std::atomic<bool> m_newFrameReady{ false };
    mutable std::mutex m_mutex;
};