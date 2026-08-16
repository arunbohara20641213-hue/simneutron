#pragma once

#include <glm/vec3.hpp>

namespace nsim::simulation {

/**
 * A single particle in the simulation.
 * Uses SI units internally.
 */
struct Particle {
    glm::dvec3 position{0.0};      // [m]
    glm::dvec3 velocity{0.0};      // [m/s]
    glm::dvec3 acceleration{0.0};  // [m/s^2]
    double mass = 0.0;             // [kg]
    double lifetime = 0.0;         // [s] remaining
    double temperature = 0.0;      // [K] for color mapping
    bool active = false;           // Whether this particle slot is in use
};

} // namespace nsim::simulation