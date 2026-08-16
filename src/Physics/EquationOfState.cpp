#include "Physics/EquationOfState.hpp"
#include "Physics/Constants.hpp"

#include <cmath>

namespace nsim::physics {

PolytropicEOS::PolytropicEOS(double K, double gamma)
    : K_(K), gamma_(gamma) {}

double PolytropicEOS::pressure(double rho) const {
    if (rho <= 0.0) {
        return 0.0;
    }
    return K_ * std::pow(rho, gamma_);
}

double PolytropicEOS::energyDensity(double rho) const {
    // Energy density includes rest mass energy plus internal energy.
    // For a polytrope: e = rho*c^2 + P/(gamma-1)
    if (rho <= 0.0) {
        return 0.0;
    }
    const double P = pressure(rho);
    return rho * c * c + P / (gamma_ - 1.0);
}

double PolytropicEOS::pressureInverse(double P) const {
    if (P <= 0.0) {
        return 0.0;
    }
    // P = K * rho^gamma  =>  rho = (P/K)^(1/gamma)
    return std::pow(P / K_, 1.0 / gamma_);
}

const char* PolytropicEOS::name() const {
    return "Polytropic P = K*rho^gamma";
}

} // namespace nsim::physics