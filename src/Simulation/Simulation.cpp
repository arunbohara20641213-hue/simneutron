#include "Simulation/Simulation.hpp"
#include "Physics/Constants.hpp"
#include "Physics/Gravity.hpp"

#include <glm/geometric.hpp>
#include <algorithm>
#include <cmath>

namespace nsim::simulation {

Simulation::Simulation()
    : particles_(20000), disk_(particles_) {
    spawnDisk();
}

void Simulation::step(double dt) {
    if (params_.paused) {
        return;
    }

    // Apply time scale
    const double scaledDt = dt * params_.timeScale;

    // Sub-stepping for stability
    const double subDt = scaledDt / static_cast<double>(params_.subSteps);
    const double clampedDt = std::min(subDt, params_.timestep);

    for (int i = 0; i < params_.subSteps; ++i) {
        // Update star rotation
        star_.update(clampedDt);

        // Update particles
        if (params_.newtonianGravity) {
            particles_.step(clampedDt, params_.method,
                            star_.mass(), star_.radius(), params_.maxVelocity);
        }

        // Update accretion disk
        if (params_.accretionDisk) {
            disk_.update(clampedDt, star_.mass(), params_.diskViscosity);
        }

        simTime_ += clampedDt;
    }

    // Check for numerical instability
    if (particles_.hasInvalidState()) {
        unstable_ = true;
    }
}

void Simulation::reset() {
    star_.reset();
    particles_.clear();
    simTime_ = 0.0;
    unstable_ = false;
    spawnDisk();
}

void Simulation::spawnDisk() {
    if (params_.accretionDisk) {
        disk_.spawnDisk(params_.diskInnerRadius, params_.diskOuterRadius,
                        params_.diskParticleCount, star_.mass(),
                        params_.diskTemperature);
    }
}

bool Simulation::isUnstable() const {
    return unstable_;
}

double Simulation::totalEnergy() const {
    double energy = 0.0;
    for (const auto& p : particles_.particles()) {
        if (!p.active) continue;

        // Kinetic energy
        const double v2 = glm::dot(p.velocity, p.velocity);
        energy += 0.5 * p.mass * v2;

        // Gravitational potential energy
        const double r = glm::length(p.position);
        if (r > 0.0) {
            energy += -physics::G * star_.mass() * p.mass / r;
        }
    }
    return energy;
}

} // namespace nsim::simulation