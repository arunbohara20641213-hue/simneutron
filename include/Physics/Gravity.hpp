#pragma once

#include <glm/vec3.hpp>

namespace nsim::physics {

/**
 * Newtonian gravity utilities.
 * All functions use SI units internally.
 */
namespace gravity {

/**
 * Gravitational acceleration magnitude: g = GM/r^2
 * @param mass Central mass [kg]
 * @param r Distance from center [m]
 * @return Acceleration magnitude [m/s^2]
 */
double accelerationMagnitude(double mass, double r);

/**
 * Gravitational acceleration vector at a position.
 * @param mass Central mass [kg]
 * @param position Position relative to center [m]
 * @return Acceleration vector [m/s^2]
 */
glm::dvec3 accelerationVector(double mass, const glm::dvec3& position);

/**
 * Gravitational potential: Phi = -GM/r
 * @param mass Central mass [kg]
 * @param r Distance from center [m]
 * @return Potential [m^2/s^2]
 */
double potential(double mass, double r);

/**
 * Escape velocity: v_esc = sqrt(2GM/r)
 * @param mass Central mass [kg]
 * @param r Distance from center [m]
 * @return Escape velocity [m/s]
 */
double escapeVelocity(double mass, double r);

/**
 * Schwarzschild radius: r_s = 2GM/c^2
 * @param mass Mass [kg]
 * @return Schwarzschild radius [m]
 */
double schwarzschildRadius(double mass);

/**
 * Compactness: C = 2GM/(Rc^2)
 * @param mass Mass [kg]
 * @param radius Radius [m]
 * @return Dimensionless compactness
 */
double compactness(double mass, double radius);

} // namespace gravity
} // namespace nsim::physics