#include <Particle/ParticleBuffer.hpp>

#include <mutex>
#include <utility>

namespace NBody::Particle {

    void ParticleBuffer::CommitBackBuffer() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(m_backBuffer, m_sharedBuffer);
        }
        m_newFrameReady.store(true);
    }

    void ParticleBuffer::UpdateFrontBuffer() {
        if (m_newFrameReady.exchange(false)) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(m_sharedBuffer, m_frontBuffer);
        }
    }

} // namespace NBody::Particle

