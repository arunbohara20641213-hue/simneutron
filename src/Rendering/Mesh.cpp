#include "Rendering/Mesh.hpp"

#include <glad/gl.h>
#include <glm/gtc/constants.hpp>

#include <cmath>
#include <utility>

namespace nsim::rendering {

Mesh::~Mesh() {
    destroy();
}

Mesh::Mesh(Mesh&& other) noexcept
    : vertices_(std::move(other.vertices_)),
      indices_(std::move(other.indices_)),
      vao_(other.vao_), vbo_(other.vbo_), ebo_(other.ebo_),
      indexCount_(other.indexCount_), uploaded_(other.uploaded_) {
    other.vao_ = other.vbo_ = other.ebo_ = 0;
    other.indexCount_ = 0;
    other.uploaded_ = false;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        destroy();
        vertices_ = std::move(other.vertices_);
        indices_ = std::move(other.indices_);
        vao_ = other.vao_; vbo_ = other.vbo_; ebo_ = other.ebo_;
        indexCount_ = other.indexCount_; uploaded_ = other.uploaded_;
        other.vao_ = other.vbo_ = other.ebo_ = 0;
        other.indexCount_ = 0;
        other.uploaded_ = false;
    }
    return *this;
}

void Mesh::createSphere(float radius, int segments, int rings) {
    vertices_.clear();
    indices_.clear();

    for (int ring = 0; ring <= rings; ++ring) {
        const float phi = glm::pi<float>() * static_cast<float>(ring) / static_cast<float>(rings);
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        for (int seg = 0; seg <= segments; ++seg) {
            const float theta = 2.0f * glm::pi<float>() * static_cast<float>(seg) / static_cast<float>(segments);
            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            const glm::vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            const glm::vec3 pos = normal * radius;

            vertices_.push_back(pos.x);
            vertices_.push_back(pos.y);
            vertices_.push_back(pos.z);
            vertices_.push_back(normal.x);
            vertices_.push_back(normal.y);
            vertices_.push_back(normal.z);
        }
    }

    // Winding matters: the renderer enables GL_CULL_FACE (back-face culling,
    // CCW front faces by default). Triangles below are ordered counter-clockwise
    // as seen from OUTSIDE the sphere so the surface stays visible.
    for (int ring = 0; ring < rings; ++ring) {
        for (int seg = 0; seg < segments; ++seg) {
            const unsigned int a = ring * (segments + 1) + seg;
            const unsigned int b = a + segments + 1;
            indices_.push_back(a);
            indices_.push_back(a + 1);
            indices_.push_back(b);
            indices_.push_back(a + 1);
            indices_.push_back(b + 1);
            indices_.push_back(b);
        }
    }
}

void Mesh::createDisk(float innerRadius, float outerRadius, int segments) {
    vertices_.clear();
    indices_.clear();

    const glm::vec3 normal(0.0f, 1.0f, 0.0f);

    // Inner ring
    for (int i = 0; i <= segments; ++i) {
        const float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
        const float c = std::cos(theta);
        const float s = std::sin(theta);
        vertices_.push_back(innerRadius * c);
        vertices_.push_back(0.0f);
        vertices_.push_back(innerRadius * s);
        vertices_.push_back(normal.x);
        vertices_.push_back(normal.y);
        vertices_.push_back(normal.z);
    }

    // Outer ring
    for (int i = 0; i <= segments; ++i) {
        const float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
        const float c = std::cos(theta);
        const float s = std::sin(theta);
        vertices_.push_back(outerRadius * c);
        vertices_.push_back(0.0f);
        vertices_.push_back(outerRadius * s);
        vertices_.push_back(normal.x);
        vertices_.push_back(normal.y);
        vertices_.push_back(normal.z);
    }

    // Winding matters: the renderer enables GL_CULL_FACE (CCW front faces).
    // Triangles are ordered counter-clockwise as seen from +Y (above) so the
    // disk is visible from the default camera, which looks down at the plane.
    for (int i = 0; i < segments; ++i) {
        const unsigned int innerA = i;
        const unsigned int innerB = i + 1;
        const unsigned int outerA = segments + 1 + i;
        const unsigned int outerB = segments + 1 + i + 1;
        indices_.push_back(innerA);
        indices_.push_back(innerB);
        indices_.push_back(outerA);
        indices_.push_back(innerB);
        indices_.push_back(outerB);
        indices_.push_back(outerA);
    }
}

void Mesh::upload() {
    if (uploaded_) {
        destroy();
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    indexCount_ = indices_.size();
    uploaded_ = true;
}

void Mesh::draw() const {
    if (!uploaded_) return;
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::destroy() {
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    uploaded_ = false;
    indexCount_ = 0;
}

} // namespace nsim::rendering