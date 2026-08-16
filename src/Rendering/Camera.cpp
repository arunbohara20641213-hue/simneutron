#include "Rendering/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

namespace nsim::rendering {

Camera::Camera() = default;

void Camera::orbit(float dx, float dy) {
    yaw_ -= dx * 0.005f;
    pitch_ += dy * 0.005f;
    pitch_ = std::clamp(pitch_, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
}

void Camera::pan(float dx, float dy) {
    const glm::vec3 right = glm::normalize(glm::cross(front_, up_));
    const glm::vec3 camUp = glm::normalize(glm::cross(right, front_));
    target_ += right * (-dx * 0.01f) + camUp * (dy * 0.01f);
}

void Camera::zoom(float amount) {
    radius_ = std::clamp(radius_ * (1.0f - amount * 0.1f), 10.0f, 1000.0f);
}

void Camera::move(float forward, float right, float up, float dt) {
    const float speed = 60.0f * dt;
    const glm::vec3 rightVec = glm::normalize(glm::cross(front_, up_));
    freePos_ += front_ * (forward * speed);
    freePos_ += rightVec * (right * speed);
    freePos_ += up_ * (up * speed);
}

void Camera::reset() {
    yaw_ = 0.0f;
    pitch_ = 0.4f;
    radius_ = 150.0f;
    target_ = glm::vec3(0.0f);
    freePos_ = glm::vec3(0.0f, 40.0f, 150.0f);
    front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    up_ = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::viewMatrix() const {
    if (freeMode_) {
        return glm::lookAt(freePos_, freePos_ + front_, up_);
    }

    // Compute orbit position from yaw/pitch/radius
    const float cp = std::cos(pitch_);
    const glm::vec3 pos(
        target_.x + radius_ * cp * std::sin(yaw_),
        target_.y + radius_ * std::sin(pitch_),
        target_.z + radius_ * cp * std::cos(yaw_)
    );
    return glm::lookAt(pos, target_, up_);
}

glm::mat4 Camera::projectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(60.0f), aspect, 0.5f, 2500.0f);
}

glm::vec3 Camera::position() const {
    if (freeMode_) {
        return freePos_;
    }
    const float cp = std::cos(pitch_);
    return glm::vec3(
        target_.x + radius_ * cp * std::sin(yaw_),
        target_.y + radius_ * std::sin(pitch_),
        target_.z + radius_ * cp * std::cos(yaw_)
    );
}

} // namespace nsim::rendering