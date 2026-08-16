#include "Physics/Relativity.hpp"
#include "Physics/Constants.hpp"
#include "Physics/Gravity.hpp"

#include <cmath>
#include <limits>

namespace nsim::physics::relativity {

double timeDilation(double mass, double r) {
    const double rs = gravity::schwarzschildRadius(mass);
    if (r <= rs) {
        return 0.0; // Inside or at horizon: time dilation factor goes to zero
    }
    return std::sqrt(1.0 - rs / r);
}

double gravitationalRedshift(double mass, double r) {
    const double rs = gravity::schwarzschildRadius(mass);
    if (r <= rs) {
        return std::numeric_limits<double>::infinity();
    }
    return 1.0 / std::sqrt(1.0 - rs / r) - 1.0;
}

} // namespace nsim::physics::relativity