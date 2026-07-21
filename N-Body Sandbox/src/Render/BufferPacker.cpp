#include <Render/BufferPacker.hpp>

#include <cstddef>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include <Particle/Particle.hpp>

namespace NBody::Render::BufferPacker {
    void packTransforms(const Particle::ParticleSystem& particleSystem, std::vector<Matrix>& outTransforms)
    {
        const std::size_t size{ particleSystem.getSize() };
        if (size == 0) return;

        if (outTransforms.size() < size) {
            outTransforms.resize(size);
        }

        for (std::size_t i = 0; i < size; ++i) {
            Matrix& mat = outTransforms[i] = MatrixIdentity();
            mat.m12 = static_cast<float>(particleSystem.positionX[i]);
            mat.m13 = static_cast<float>(particleSystem.positionY[i]);
            mat.m14 = static_cast<float>(particleSystem.positionZ[i]);

            const float r{ particleSystem.radius[i] };
            mat.m0 = r; mat.m5 = r; mat.m10 = r;
        }
    }

    void PackColors(const Particle::ParticleSystem& particleSystem, std::vector<Vector4>& outColors)
    {
        const std::size_t size{ particleSystem.getSize() };
        if (size == 0) return;

        if (outColors.size() < size) {
            outColors.resize(size);
        }

        for (std::size_t i = 0; i < size; ++i) {
            Vector4& col{ outColors[i] };
            col.x = static_cast<float>(particleSystem.color[i].r) / 255.0f;
            col.y = static_cast<float>(particleSystem.color[i].g) / 255.0f;
            col.z = static_cast<float>(particleSystem.color[i].b) / 255.0f;
            col.w = static_cast<float>(particleSystem.color[i].a) / 255.0f;
        }
    }
}

