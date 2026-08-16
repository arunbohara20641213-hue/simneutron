#pragma once

#include "Simulation/Particle.hpp"

#include <cstddef>
#include <vector>

#include <glm/vec3.hpp>

namespace nsim::simulation {

/**
 * Integration methods for particle motion.
 */
enum class IntegrationMethod {
    Euler,
    Verlet,
    RK4
};

/**
 * Manages a preallocated pool of particles.
 * Uses a fixed-size array to avoid heap allocation during simulation.
 */
class ParticleSystem {
public:
    /**
     * @param capacity Maximum number of particles
     */
    explicit ParticleSystem(std::size_t capacity);

    /**
     * Spawn a particle at a position with a velocity.
     * @return Index of the new particle, or -1 if full
     */
    int spawn(const glm::dvec3& position, const glm::dvec3& velocity,
              double mass, double lifetime, double temperature);

    /**
     * Remove a particle by index.
     */
    void remove(std::size_t index);

    /**
     * Clear all particles.
     */
    void clear();

    /**
     * Advance the simulation by dt seconds.
     * @param dt Timestep [s]
     * @param method Integration method
     * @param starMass Central mass [kg]
     * @param starRadius Star radius [m] (particles inside are removed)
     * @param maxVelocity Maximum allowed velocity [m/s]
     */
    void step(double dt, IntegrationMethod method,
              double starMass, double starRadius, double maxVelocity);

    /**
     * Compute gravitational acceleration for a particle.
     * @param p Particle
     * @param starMass Central mass [kg]
     * @return Acceleration [m/s^2]
     */
    glm::dvec3 computeAcceleration(const Particle& p, double starMass) const;

    /**
     * Check for NaN/Inf in particle state.
     * @return true if any particle has invalid values
     */
    bool hasInvalidState() const;

    /**
     * Number of active particles.
     */
    std::size_t activeCount() const;

    /**
     * Access the particle pool.
     */
    const std::vector<Particle>& particles() const { return particles_; }
    std::vector<Particle>& particles() { return particles_; }

    /**
     * Capacity of the pool.
     */
    std::size_t capacity() const { return particles_.size(); }

private:
    void stepEuler(double dt, double starMass, double starRadius, double maxVelocity);
    void stepVerlet(double dt, double starMass, double starRadius, double maxVelocity);
    void stepRK4(double dt, double starMass, double starRadius, double maxVelocity);

    void applyBoundary(Particle& p, double starRadius, double maxVelocity);

    std::vector<Particle> particles_;
};

} // namespace nsim::simulation