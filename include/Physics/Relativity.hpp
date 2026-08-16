#pragma once

namespace nsim::physics {

/**
 * Relativistic effects (Schwarzschild/static-field approximations).
 * These are educational approximations, not full general relativity.
 */
namespace relativity {

/**
 * Gravitational time dilation factor: dtau/dt = sqrt(1 - 2GM/(rc^2))
 * @param mass Central mass [kg]
 * @param r Radial distance from center [m]
 * @return Time dilation factor (0..1). Returns 0 if r <= r_s.
 */
double timeDilation(double mass, double r);

/**
 * Gravitational redshift: z = 1/sqrt(1 - 2GM/(rc^2)) - 1
 * @param mass Central mass [kg]
 * @param r Radial distance from center [m]
 * @return Redshift z. Returns infinity if r <= r_s.
 */
double gravitationalRedshift(double mass, double r);

} // namespace relativity
} // namespace nsim::physics