#include "Simulation/NeutronStar.hpp"
#include "Physics/Constants.hpp"
#include "Physics/Rotation.hpp"

#include <cmath>

namespace nsim::simulation {

NeutronStar::NeutronStar()
    : mass_(physics::DEFAULT_STAR_MASS),
      radius_(physics::DEFAULT_STAR_RADIUS),
      omega_(0.0),
      direction_(1.0),
      inclination_(0.0),
      azimuth_(0.0),
      rotationAngle_(0.0) {}

glm::dvec3 NeutronStar::axis() const {
    return physics::rotation::axisFromAngles(inclination_, azimuth_);
}

void NeutronStar::update(double dt) {
    rotationAngle_ += direction_ * omega_ * dt;
    // Keep angle bounded to avoid precision loss
    constexpr double twoPi = 6.28318530717958647692;
    rotationAngle_ = std::fmod(rotationAngle_, twoPi);
}

void NeutronStar::reset() {
    mass_ = physics::DEFAULT_STAR_MASS;
    radius_ = physics::DEFAULT_STAR_RADIUS;
    omega_ = 0.0;
    direction_ = 1.0;
    inclination_ = 0.0;
    azimuth_ = 0.0;
    rotationAngle_ = 0.0;
}

} // namespace nsim::simulation