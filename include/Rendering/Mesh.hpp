#pragma once

#include <glm/vec3.hpp>

#include <cstddef>
#include <vector>

namespace nsim::rendering {

/**
 * Simple mesh with position and normal data.
 * Used for the star sphere and disk.
 */
class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    /**
     * Create a UV sphere mesh.
     * @param radius Sphere radius
     * @param segments Number of segments
     * @param rings Number of rings
     */
    void createSphere(float radius, int segments, int rings);

    /**
     * Create a flat disk mesh (ring).
     * @param innerRadius Inner radius
     * @param outerRadius Outer radius
     * @param segments Number of segments
     */
    void createDisk(float innerRadius, float outerRadius, int segments);

    /**
     * Upload vertex data to GPU.
     */
    void upload();

    /**
     * Draw the mesh.
     */
    void draw() const;

    /**
     * Free GPU resources.
     */
    void destroy();

private:
    std::vector<float> vertices_;  // interleaved: pos(3) + normal(3)
    std::vector<unsigned int> indices_;

    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
    std::size_t indexCount_ = 0;
    bool uploaded_ = false;
};

} // namespace nsim::rendering