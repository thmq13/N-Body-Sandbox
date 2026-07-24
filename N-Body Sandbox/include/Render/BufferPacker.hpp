#pragma once

#include <vector>

#include <raylib.h>

namespace NBody::Particle { struct ParticleSystem; }

namespace NBody::Render::BufferPacker {
    void packTransforms(const Particle::ParticleSystem& particleSystem, std::vector<Matrix>& outTransforms);
    void packColors(const Particle::ParticleSystem& particleSystem, std::vector<Vector4>& outColors);
}