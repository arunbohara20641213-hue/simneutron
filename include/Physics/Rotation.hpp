#pragma once

#include <glm/vec3.hpp>

namespace nsim::physics {

/**
 * Rotation utilities for the neutron star.
 */
namespace rotation {

/**
 * Rotational velocity magnitude: v = omega * r
 * @param omega Angular velocity [rad/s]
 * @param r Distance from rotation axis [m]
 * @return Tangential velocity [m/s]
 */
double tangentialVelocity(double omega, double r);

/**
 * Check if a rotation rate is physically unreasonable.
 * Warns when the equatorial velocity approaches the speed of light.
 * @param omega Angular velocity [rad/s]
 * @param radius Star radius [m]
 * @return true if the rotation rate is physically unreasonable
 */
bool isPhysicallyUnreasonable(double omega, double radius);

/**
 * Compute the rotation axis direction from inclination.
 * @param inclination Inclination angle from +Y axis [radians]
 * @param azimuth Azimuth angle in XZ plane [radians]
 * @return Unit vector along rotation axis
 */
glm::dvec3 axisFromAngles(double inclination, double azimuth);

} // namespace rotation
} // namespace nsim::physics