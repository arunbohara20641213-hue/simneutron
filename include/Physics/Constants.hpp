#pragma once

namespace nsim::physics {

// Physical constants (SI units)
inline constexpr double G = 6.67430e-11;        // Gravitational constant [m^3 kg^-1 s^-2]
inline constexpr double c = 299792458.0;        // Speed of light [m/s]
inline constexpr double M_sun = 1.98847e30;     // Solar mass [kg]
inline constexpr double R_sun = 6.957e8;        // Solar radius [m]

// Default neutron star parameters
inline constexpr double DEFAULT_STAR_MASS = 1.4 * M_sun;  // 1.4 solar masses [kg]
inline constexpr double DEFAULT_STAR_RADIUS = 12000.0;    // 12 km [m]

// Simulation scale: 1 world unit = 1 km
inline constexpr double KM_TO_M = 1000.0;
inline constexpr double M_TO_KM = 1.0 / KM_TO_M;

} // namespace nsim::physics