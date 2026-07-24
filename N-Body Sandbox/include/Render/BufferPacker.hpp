#pragma once

#include <vector>

#include <raylib.h>

namespace NBody::Particle { struct ParticleSystem; }

namespace NBody::Render::BufferPacker {

    void PackTransforms(const Particle::ParticleSystem& particleSystem, std::vector<Matrix>& outTransforms);
    void PackColors(const Particle::ParticleSystem& particleSystem, std::vector<Vector4>& outColors);

} // namespace NBody::Render::BufferPacker