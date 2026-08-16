#define _USE_MATH_DEFINES
#include "Simulation/AccretionDisk.hpp"
#include "Physics/Constants.hpp"
#include "Physics/Gravity.hpp"

#include <glm/geometric.hpp>
#include <cmath>
#include <random>

namespace nsim::simulation {

AccretionDisk::AccretionDisk(ParticleSystem& particleSystem)
    : particles_(particleSystem) {}

void AccretionDisk::spawnDisk(double innerRadius, double outerRadius, int count,
                              double starMass, double temperature) {
    if (innerRadius <= 0.0 || outerRadius <= innerRadius || count <= 0) {
        return;
    }

    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<double> radiusDist(innerRadius, outerRadius);
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> heightDist(-0.05, 0.05);

    for (int i = 0; i < count; ++i) {
        const double r = radiusDist(rng);
        const double theta = angleDist(rng);

        // Position in the XZ plane (disk plane)
        const glm::dvec3 pos(
            r * std::cos(theta),
            heightDist(rng) * r, // Thin disk
            r * std::sin(theta)
        );

        // Circular orbital velocity: v = sqrt(GM/r)
        const double v_orb = std::sqrt(physics::G * starMass / r);
        const glm::dvec3 vel(
            -v_orb * std::sin(theta),
            0.0,
            v_orb * std::cos(theta)
        );

        // Temperature gradient: hotter near the star
        const double localTemp = temperature * std::sqrt(innerRadius / r);

        particles_.spawn(pos, vel, 1.0, 1e9, localTemp);
    }
}

void AccretionDisk::update(double dt, double starMass, double viscosity) {
    if (!enabled_) return;

    for (auto& p : particles_.particles()) {
        if (!p.active) continue;

        // Approximate inward drift due to viscosity
        const double r = glm::length(p.position);
        if (r > 0.0) {
            const double drift = viscosity * dt * 100.0; // Small inward drift
            p.position -= (p.position / r) * drift;
        }
    }
}

void AccretionDisk::clear() {
    // Remove all particles that were spawned by the disk
    // (In practice, we just clear the whole system)
    particles_.clear();
}

std::size_t AccretionDisk::activeCount() const {
    return particles_.activeCount();
}

} // namespace nsim::simulation