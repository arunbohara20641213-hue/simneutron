#include "Physics/Gravity.hpp"
#include "Physics/Constants.hpp"

#include <glm/geometric.hpp>
#include <cmath>
#include <limits>

namespace nsim::physics::gravity {

double accelerationMagnitude(double mass, double r) {
    if (r <= 0.0) {
        return 0.0; // Avoid division by zero at center
    }
    return G * mass / (r * r);
}

glm::dvec3 accelerationVector(double mass, const glm::dvec3& position) {
    const double r = glm::length(position);
    if (r <= 0.0) {
        return glm::dvec3(0.0);
    }
    // a = -GM/r^2 * r_hat = -GM/r^3 * r
    const double factor = -G * mass / (r * r * r);
    return factor * position;
}

double potential(double mass, double r) {
    if (r <= 0.0) {
        return -std::numeric_limits<double>::infinity();
    }
    return -G * mass / r;
}

double escapeVelocity(double mass, double r) {
    if (r <= 0.0) {
        return 0.0;
    }
    return std::sqrt(2.0 * G * mass / r);
}

double schwarzschildRadius(double mass) {
    return 2.0 * G * mass / (c * c);
}

double compactness(double mass, double radius) {
    if (radius <= 0.0) {
        return 0.0;
    }
    return 2.0 * G * mass / (radius * c * c);
}

} // namespace nsim::physics::gravity