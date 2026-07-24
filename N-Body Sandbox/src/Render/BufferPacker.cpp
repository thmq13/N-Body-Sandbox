#include <Render/BufferPacker.hpp>

#include <cstddef>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include <Particle/Particle.hpp>

namespace NBody::Render::BufferPacker {

    void PackTransforms(const Particle::ParticleSystem& particleSystem, std::vector<Matrix>& outTransforms) {
        const std::size_t size{ particleSystem.GetSize() };
        outTransforms.resize(size);

        for (std::size_t i = 0; i < size; ++i) {
            Matrix& mat = outTransforms[i] = MatrixIdentity();
            mat.m12 = static_cast<float>(particleSystem.positionX[i]);
            mat.m13 = static_cast<float>(particleSystem.positionY[i]);
            mat.m14 = static_cast<float>(particleSystem.positionZ[i]);

            const float r{ particleSystem.radius[i] };
            mat.m0 = r; mat.m5 = r; mat.m10 = r;
        }
    }

    void PackColors(const Particle::ParticleSystem& particleSystem, std::vector<Vector4>& outColors) {
        const std::size_t size{ particleSystem.GetSize() };
        outColors.resize(size);

        constexpr float inv255{ 1.0f / 255.0f };

        for (std::size_t i = 0; i < size; ++i) {
            Vector4& col{ outColors[i] };
            col.x = static_cast<float>(particleSystem.color[i].r) * inv255;
            col.y = static_cast<float>(particleSystem.color[i].g) * inv255;
            col.z = static_cast<float>(particleSystem.color[i].b) * inv255;
            col.w = static_cast<float>(particleSystem.color[i].a) * inv255;
        }
    }

} // namespace NBody::Render::BufferPacker

