#pragma once

#include "Simulation/ParticleSystem.hpp"
#include "Simulation/AccretionDisk.hpp"
#include "Simulation/NeutronStar.hpp"
#include "Physics/Constants.hpp"

#include <glm/vec3.hpp>

namespace nsim::simulation {

/**
 * Simulation parameters exposed to the UI.
 */
struct SimulationParams {
    // Simulation control
    bool paused = false;
    double timeScale = 1.0;
    double timestep = 0.01;          // [s]
    IntegrationMethod method = IntegrationMethod::Verlet;
    int subSteps = 4;

    // Physics toggles
    bool newtonianGravity = true;
    bool relativisticEffects = true;
    bool photonRays = false;
    bool accretionDisk = true;

    // Disk parameters
    double diskInnerRadius = 2.0e4;  // [m]
    double diskOuterRadius = 1.0e5;  // [m]
    int diskParticleCount = 5000;
    double diskTemperature = 1.0e6;  // [K]
    double diskViscosity = 0.001;

    // Numerical stability
    double maxVelocity = 0.5 * physics::c; // Cap at 50% of light speed
};

/**
 * Top-level simulation orchestrator.
 * Owns the star, particle system, and accretion disk.
 */
class Simulation {
public:
    Simulation();

    /**
     * Advance the simulation by dt seconds.
     * @param dt Real elapsed time [s]
     */
    void step(double dt);

    /**
     * Reset the simulation to initial state.
     */
    void reset();

    /**
     * Spawn the accretion disk.
     */
    void spawnDisk();

    /**
     * Check for numerical instability.
     * @return true if the simulation has become unstable
     */
    bool isUnstable() const;

    /**
     * Total simulation time [s].
     */
    double simulationTime() const { return simTime_; }

    /**
     * Total energy of all particles [J].
     */
    double totalEnergy() const;

    /**
     * Access components.
     */
    NeutronStar& star() { return star_; }
    const NeutronStar& star() const { return star_; }
    ParticleSystem& particles() { return particles_; }
    const ParticleSystem& particles() const { return particles_; }
    AccretionDisk& disk() { return disk_; }
    const AccretionDisk& disk() const { return disk_; }
    SimulationParams& params() { return params_; }
    const SimulationParams& params() const { return params_; }

private:
    NeutronStar star_;
    ParticleSystem particles_;
    AccretionDisk disk_;
    SimulationParams params_;
    double simTime_ = 0.0;
    bool unstable_ = false;
};

} // namespace nsim::simulation