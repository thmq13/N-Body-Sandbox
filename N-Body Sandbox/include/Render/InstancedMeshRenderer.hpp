#pragma once

#include <cstddef>
#include <vector>

#include <raylib.h>

namespace NBody::Particle { struct ParticleSystem; }

namespace NBody::Render {
    class InstancedMeshRenderer {
    public:
        InstancedMeshRenderer();
        ~InstancedMeshRenderer();

        InstancedMeshRenderer(const InstancedMeshRenderer&) = delete;
        InstancedMeshRenderer& operator=(const InstancedMeshRenderer&) = delete;

        void draw(const Particle::ParticleSystem& particleSystem);

    private:
        std::vector<Matrix> m_transforms{};
        std::vector<Vector4> m_colors{};

        unsigned int m_colorVboId{ 0U };
        std::size_t m_vboCapacity{ 0 };

        Mesh m_baseMesh{};
        Material m_material{};
        int m_colorAttributeLocation{ -1 };

        void reinitializeVertexBuffer(std::size_t size);
    };
}
