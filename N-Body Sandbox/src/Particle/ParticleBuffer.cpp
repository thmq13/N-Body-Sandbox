#include <Particle/ParticleBuffer.hpp>

namespace NBody::Particle {
    void ParticleBuffer::commitBackBuffer() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(m_backBuffer, m_sharedBuffer);
        }
        m_newFrameReady.store(true);
    }

    void ParticleBuffer::updateFrontBuffer() {
        if (m_newFrameReady.exchange(false)) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(m_sharedBuffer, m_frontBuffer);
        }
    }
}

