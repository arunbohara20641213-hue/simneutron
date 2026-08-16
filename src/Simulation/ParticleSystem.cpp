#include "Simulation/ParticleSystem.hpp"
#include "Physics/Gravity.hpp"
#include "Physics/Constants.hpp"

#include <glm/geometric.hpp>
#include <algorithm>
#include <cmath>

namespace nsim::simulation {

ParticleSystem::ParticleSystem(std::size_t capacity)
    : particles_(capacity) {}

int ParticleSystem::spawn(const glm::dvec3& position, const glm::dvec3& velocity,
                          double mass, double lifetime, double temperature) {
    for (std::size_t i = 0; i < particles_.size(); ++i) {
        if (!particles_[i].active) {
            auto& p = particles_[i];
            p.position = position;
            p.velocity = velocity;
            p.acceleration = glm::dvec3(0.0);
            p.mass = mass;
            p.lifetime = lifetime;
            p.temperature = temperature;
            p.active = true;
            return static_cast<int>(i);
        }
    }
    return -1; // Pool full
}

void ParticleSystem::remove(std::size_t index) {
    if (index < particles_.size()) {
        particles_[index].active = false;
    }
}

void ParticleSystem::clear() {
    for (auto& p : particles_) {
        p.active = false;
    }
}

glm::dvec3 ParticleSystem::computeAcceleration(const Particle& p, double starMass) const {
    return physics::gravity::accelerationVector(starMass, p.position);
}

void ParticleSystem::applyBoundary(Particle& p, double starRadius, double maxVelocity) {
    // Remove particles that fall inside the star
    const double r = glm::length(p.position);
    if (r < starRadius) {
        p.active = false;
        return;
    }

    // Clamp velocity to prevent numerical explosion
    const double v = glm::length(p.velocity);
    if (v > maxVelocity) {
        p.velocity = (p.velocity / v) * maxVelocity;
    }
}

void ParticleSystem::stepEuler(double dt, double starMass, double starRadius, double maxVelocity) {
    for (auto& p : particles_) {
        if (!p.active) continue;

        p.acceleration = computeAcceleration(p, starMass);
        p.velocity += p.acceleration * dt;
        p.position += p.velocity * dt;
        p.lifetime -= dt;

        applyBoundary(p, starRadius, maxVelocity);
        if (p.lifetime <= 0.0) {
            p.active = false;
        }
    }
}

void ParticleSystem::stepVerlet(double dt, double starMass, double starRadius, double maxVelocity) {
    for (auto& p : particles_) {
        if (!p.active) continue;

        // Velocity Verlet
        const glm::dvec3 a0 = computeAcceleration(p, starMass);
        p.position += p.velocity * dt + 0.5 * a0 * dt * dt;
        const glm::dvec3 a1 = computeAcceleration(p, starMass);
        p.velocity += 0.5 * (a0 + a1) * dt;
        p.lifetime -= dt;

        applyBoundary(p, starRadius, maxVelocity);
        if (p.lifetime <= 0.0) {
            p.active = false;
        }
    }
}

void ParticleSystem::stepRK4(double dt, double starMass, double starRadius, double maxVelocity) {
    for (auto& p : particles_) {
        if (!p.active) continue;

        // RK4 integration
        const glm::dvec3 k1v = computeAcceleration(p, starMass);
        const glm::dvec3 k1x = p.velocity;

        const glm::dvec3 v2 = p.velocity + 0.5 * dt * k1v;
        const glm::dvec3 x2 = p.position + 0.5 * dt * k1x;
        Particle tmp2 = p;
        tmp2.position = x2;
        tmp2.velocity = v2;
        const glm::dvec3 k2v = computeAcceleration(tmp2, starMass);
        const glm::dvec3 k2x = v2;

        const glm::dvec3 v3 = p.velocity + 0.5 * dt * k2v;
        const glm::dvec3 x3 = p.position + 0.5 * dt * k2x;
        Particle tmp3 = p;
        tmp3.position = x3;
        tmp3.velocity = v3;
        const glm::dvec3 k3v = computeAcceleration(tmp3, starMass);
        const glm::dvec3 k3x = v3;

        const glm::dvec3 v4 = p.velocity + dt * k3v;
        const glm::dvec3 x4 = p.position + dt * k3x;
        Particle tmp4 = p;
        tmp4.position = x4;
        tmp4.velocity = v4;
        const glm::dvec3 k4v = computeAcceleration(tmp4, starMass);
        const glm::dvec3 k4x = v4;

        p.position += (dt / 6.0) * (k1x + 2.0 * k2x + 2.0 * k3x + k4x);
        p.velocity += (dt / 6.0) * (k1v + 2.0 * k2v + 2.0 * k3v + k4v);
        p.lifetime -= dt;

        applyBoundary(p, starRadius, maxVelocity);
        if (p.lifetime <= 0.0) {
            p.active = false;
        }
    }
}

void ParticleSystem::step(double dt, IntegrationMethod method,
                          double starMass, double starRadius, double maxVelocity) {
    switch (method) {
        case IntegrationMethod::Euler:
            stepEuler(dt, starMass, starRadius, maxVelocity);
            break;
        case IntegrationMethod::Verlet:
            stepVerlet(dt, starMass, starRadius, maxVelocity);
            break;
        case IntegrationMethod::RK4:
            stepRK4(dt, starMass, starRadius, maxVelocity);
            break;
    }
}

bool ParticleSystem::hasInvalidState() const {
    for (const auto& p : particles_) {
        if (!p.active) continue;
        if (!std::isfinite(p.position.x) || !std::isfinite(p.position.y) || !std::isfinite(p.position.z) ||
            !std::isfinite(p.velocity.x) || !std::isfinite(p.velocity.y) || !std::isfinite(p.velocity.z)) {
            return true;
        }
    }
    return false;
}

std::size_t ParticleSystem::activeCount() const {
    return static_cast<std::size_t>(
        std::count_if(particles_.begin(), particles_.end(),
                      [](const Particle& p) { return p.active; }));
}

} // namespace nsim::simulation