#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace nsim::rendering {

/**
 * Orbit camera with pan, zoom, and free-camera modes.
 */
class Camera {
public:
    Camera();

    /**
     * Orbit camera: rotate around target.
     */
    void orbit(float dx, float dy);

    /**
     * Pan the camera target.
     */
    void pan(float dx, float dy);

    /**
     * Zoom (change orbit radius).
     */
    void zoom(float amount);

    /**
     * Free camera movement (WASD).
     */
    void move(float forward, float right, float up, float dt);

    /**
     * Reset to default position.
     */
    void reset();

    /**
     * Toggle between orbit and free camera modes.
     */
    void toggleMode() { freeMode_ = !freeMode_; }
    bool freeMode() const { return freeMode_; }

    /**
     * View matrix.
     */
    glm::mat4 viewMatrix() const;

    /**
     * Projection matrix.
     */
    glm::mat4 projectionMatrix(float aspect) const;

    /**
     * Camera position.
     */
    glm::vec3 position() const;

    /**
     * Camera target (orbit mode).
     */
    glm::vec3 target() const { return target_; }

    /**
     * Orbit radius.
     */
    float radius() const { return radius_; }

private:
    // Orbit parameters (world units: 1 unit = 1 km)
    float yaw_ = 0.0f;
    float pitch_ = 0.4f;
    float radius_ = 150.0f;
    glm::vec3 target_{0.0f};

    // Free camera
    glm::vec3 freePos_{0.0f, 40.0f, 150.0f};
    bool freeMode_ = false;

    // Shared
    glm::vec3 front_{0.0f, 0.0f, -1.0f};
    glm::vec3 up_{0.0f, 1.0f, 0.0f};
};

} // namespace nsim::rendering