#pragma once

#include <glm/vec3.hpp>

namespace nsim::simulation {

/**
 * State of the neutron star in the simulation.
 */
class NeutronStar {
public:
    NeutronStar();

    /**
     * Set mass [kg].
     */
    void setMass(double mass) { mass_ = mass; }
    double mass() const { return mass_; }

    /**
     * Set radius [m].
     */
    void setRadius(double radius) { radius_ = radius; }
    double radius() const { return radius_; }

    /**
     * Set angular velocity [rad/s].
     */
    void setAngularVelocity(double omega) { omega_ = omega; }
    double angularVelocity() const { return omega_; }

    /**
     * Set rotation direction (+1 or -1).
     */
    void setRotationDirection(double dir) { direction_ = dir; }
    double rotationDirection() const { return direction_; }

    /**
     * Set inclination angle [radians].
     */
    void setInclination(double inclination) { inclination_ = inclination; }
    double inclination() const { return inclination_; }

    /**
     * Set azimuth angle [radians].
     */
    void setAzimuth(double azimuth) { azimuth_ = azimuth; }
    double azimuth() const { return azimuth_; }

    /**
     * Rotation axis unit vector.
     */
    glm::dvec3 axis() const;

    /**
     * Current rotation angle [radians].
     */
    double rotationAngle() const { return rotationAngle_; }

    /**
     * Advance rotation by dt.
     */
    void update(double dt);

    /**
     * Reset to defaults.
     */
    void reset();

private:
    double mass_;
    double radius_;
    double omega_;
    double direction_;
    double inclination_;
    double azimuth_;
    double rotationAngle_;
};

} // namespace nsim::simulation