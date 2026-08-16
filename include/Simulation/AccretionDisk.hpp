#pragma once

#include "Simulation/ParticleSystem.hpp"

#include <glm/vec3.hpp>

namespace nsim::simulation {

/**
 * Accretion disk model using particles.
 * Particles follow approximate orbital motion with inward drift.
 * This is NOT a full magnetohydrodynamic simulation.
 */
class AccretionDisk {
public:
    /**
     * @param particleSystem Reference to the shared particle pool
     */
    explicit AccretionDisk(ParticleSystem& particleSystem);

    /**
     * Spawn particles in a disk around the star.
     * @param innerRadius Inner disk radius [m]
     * @param outerRadius Outer disk radius [m]
     * @param count Number of particles to spawn
     * @param starMass Central mass [kg]
     * @param temperature Disk temperature [K]
     */
    void spawnDisk(double innerRadius, double outerRadius, int count,
                   double starMass, double temperature);

    /**
     * Apply orbital motion and inward drift to disk particles.
     * @param dt Timestep [s]
     * @param starMass Central mass [kg]
     * @param viscosity Inward drift strength (0..1)
     */
    void update(double dt, double starMass, double viscosity);

    /**
     * Clear all disk particles.
     */
    void clear();

    /**
     * Number of active disk particles.
     */
    std::size_t activeCount() const;

    /**
     * Whether the disk is enabled.
     */
    bool enabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }

private:
    ParticleSystem& particles_;
    bool enabled_ = true;
};

} // namespace nsim::simulation