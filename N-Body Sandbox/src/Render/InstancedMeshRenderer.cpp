#include <Render/InstancedMeshRenderer.hpp>

#include <stdexcept>
#include <cstddef>

#include <rlgl.h>

#include <Particle/Particle.hpp>
#include <Render/BufferPacker.hpp>

namespace NBody::Render {
    InstancedMeshRenderer::InstancedMeshRenderer() {
        m_baseMesh = GenMeshSphere(1.0f, 4, 4);
        m_material = LoadMaterialDefault();
        m_material.shader = LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl");
        m_colorAttributeLocation = GetShaderLocationAttrib(m_material.shader, "instanceColor");
    }

    InstancedMeshRenderer::~InstancedMeshRenderer() {
        if (m_colorVboId != 0) rlUnloadVertexBuffer(m_colorVboId);
        UnloadMaterial(m_material);
        UnloadMesh(m_baseMesh);
    }

    void InstancedMeshRenderer::draw(const Particle::ParticleSystem& particleSystem) {
        const std::size_t size{ particleSystem.getSize() };
        if (size == 0) return;

        BufferPacker::packTransforms(particleSystem, m_transforms);
        BufferPacker::packColors(particleSystem, m_colors);

        if (size > m_vboCapacity) {
            reinitializeVertexBuffer(size);
        }
        else {
            rlUpdateVertexBuffer(m_colorVboId, m_colors.data(), static_cast<int>(size * sizeof(Vector4)), 0);
        }

        DrawMeshInstanced(m_baseMesh, m_material, m_transforms.data(), static_cast<int>(size));
    }

    void InstancedMeshRenderer::reinitializeVertexBuffer(std::size_t size) {
        if (m_colorVboId != 0) rlUnloadVertexBuffer(m_colorVboId);

        rlEnableVertexArray(m_baseMesh.vaoId);

        m_colorVboId = rlLoadVertexBuffer(m_colors.data(), static_cast<int>(size * sizeof(Vector4)), true);

        rlEnableVertexBuffer(m_colorVboId);

        rlSetVertexAttribute(m_colorAttributeLocation, 4, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(m_colorAttributeLocation);
        rlSetVertexAttributeDivisor(m_colorAttributeLocation, 1);

        rlDisableVertexBuffer();
        rlDisableVertexArray();

        m_vboCapacity = size;
    }
}


