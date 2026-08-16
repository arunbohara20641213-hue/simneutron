#include "Physics/Rotation.hpp"
#include "Physics/Constants.hpp"

#include <cmath>

namespace nsim::physics::rotation {

double tangentialVelocity(double omega, double r) {
    return omega * r;
}

bool isPhysicallyUnreasonable(double omega, double radius) {
    // Warn if equatorial velocity exceeds ~10% of the speed of light
    const double v_eq = std::abs(omega) * radius;
    return v_eq > 0.1 * c;
}

glm::dvec3 axisFromAngles(double inclination, double azimuth) {
    // Spherical coordinates: inclination from +Y, azimuth in XZ plane
    const double sinInc = std::sin(inclination);
    return glm::dvec3(
        sinInc * std::cos(azimuth),
        std::cos(inclination),
        sinInc * std::sin(azimuth)
    );
}

} // namespace nsim::physics::rotation