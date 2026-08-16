#include "Physics/TOVSolver.hpp"
#include "Physics/Constants.hpp"
#include "Physics/Gravity.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace nsim::physics {

namespace {

// Pressure threshold relative to central pressure to stop integration
constexpr double PRESSURE_FRACTION = 1e-6;

// Maximum number of radial steps to prevent runaway
constexpr int MAX_STEPS = 100000;

} // namespace

TOVSolver::TOVSolver(const EquationOfState& eos, double dr)
    : eos_(eos), dr_(dr) {}

TOVResult TOVSolver::solve(double centralDensity) const {
    TOVResult result;
    result.centralDensity = centralDensity;
    result.centralPressure = eos_.pressure(centralDensity);

    if (centralDensity <= 0.0 || result.centralPressure <= 0.0) {
        return result; // Invalid input
    }

    // Start slightly off-center to avoid singularity at r=0
    const double r0 = dr_ * 0.5;
    double r = r0;
    double m = (4.0 / 3.0) * M_PI * r0 * r0 * r0 * centralDensity;
    double P = result.centralPressure;

    result.r.push_back(r);
    result.density.push_back(centralDensity);
    result.pressure.push_back(P);
    result.massProfile.push_back(m);

    const double P_stop = result.centralPressure * PRESSURE_FRACTION;

    for (int step = 0; step < MAX_STEPS; ++step) {
        const double rho = eos_.pressureInverse(P); // density at current pressure

        // TOV equations
        // dm/dr = 4*pi*r^2*rho
        // dP/dr = -G*(rho + P/c^2)*(m + 4*pi*r^3*P/c^2) / (r^2*(1 - 2Gm/(rc^2)))
        const double r2 = r * r;
        const double r3 = r2 * r;
        const double Pc2 = P / (c * c);

        const double dmdr = 4.0 * M_PI * r2 * rho;
        const double denom = r2 * (1.0 - 2.0 * G * m / (r * c * c));
        if (denom <= 0.0) {
            // Reached the Schwarzschild radius - star would collapse
            result.collapsed = true;
            break;
        }
        const double dPdr = -G * (rho + Pc2) * (m + 4.0 * M_PI * r3 * Pc2) / denom;

        // RK4 integration
        const double k1_m = dmdr;
        const double k1_P = dPdr;

        const double r_mid = r + 0.5 * dr_;
        const double m_mid = m + 0.5 * dr_ * k1_m;
        const double P_mid = P + 0.5 * dr_ * k1_P;
        const double rho_mid = eos_.pressureInverse(P_mid);
        const double r2_mid = r_mid * r_mid;
        const double r3_mid = r2_mid * r_mid;
        const double Pc2_mid = P_mid / (c * c);
        const double denom_mid = r2_mid * (1.0 - 2.0 * G * m_mid / (r_mid * c * c));
        if (denom_mid <= 0.0) {
            result.collapsed = true;
            break;
        }
        const double k2_m = 4.0 * M_PI * r2_mid * rho_mid;
        const double k2_P = -G * (rho_mid + Pc2_mid) * (m_mid + 4.0 * M_PI * r3_mid * Pc2_mid) / denom_mid;

        const double m_mid2 = m + 0.5 * dr_ * k2_m;
        const double P_mid2 = P + 0.5 * dr_ * k2_P;
        const double rho_mid2 = eos_.pressureInverse(P_mid2);
        const double r2_mid2 = r_mid * r_mid;
        const double r3_mid2 = r2_mid2 * r_mid;
        const double Pc2_mid2 = P_mid2 / (c * c);
        const double denom_mid2 = r2_mid2 * (1.0 - 2.0 * G * m_mid2 / (r_mid * c * c));
        if (denom_mid2 <= 0.0) {
            result.collapsed = true;
            break;
        }
        const double k3_m = 4.0 * M_PI * r2_mid2 * rho_mid2;
        const double k3_P = -G * (rho_mid2 + Pc2_mid2) * (m_mid2 + 4.0 * M_PI * r3_mid2 * Pc2_mid2) / denom_mid2;

        const double r_next = r + dr_;
        const double m_next = m + dr_ * k3_m;
        const double P_next = P + dr_ * k3_P;
        const double rho_next = eos_.pressureInverse(P_next);
        const double r2_next = r_next * r_next;
        const double r3_next = r2_next * r_next;
        const double Pc2_next = P_next / (c * c);
        const double denom_next = r2_next * (1.0 - 2.0 * G * m_next / (r_next * c * c));
        if (denom_next <= 0.0) {
            result.collapsed = true;
            break;
        }
        const double k4_m = 4.0 * M_PI * r2_next * rho_next;
        const double k4_P = -G * (rho_next + Pc2_next) * (m_next + 4.0 * M_PI * r3_next * Pc2_next) / denom_next;

        const double m_new = m + (dr_ / 6.0) * (k1_m + 2.0 * k2_m + 2.0 * k3_m + k4_m);
        const double P_new = P + (dr_ / 6.0) * (k1_P + 2.0 * k2_P + 2.0 * k3_P + k4_P);

        r = r_next;
        m = m_new;
        P = P_new;

        if (P <= P_stop || P <= 0.0) {
            // Surface reached
            result.radius = r;
            result.mass = m;
            result.surfacePressure = P;
            result.converged = true;
            result.compactness = gravity::compactness(m, r);
            break;
        }

        if (!std::isfinite(P) || !std::isfinite(m) || P > result.centralPressure) {
            // Numerical instability
            break;
        }

        result.r.push_back(r);
        result.density.push_back(eos_.pressureInverse(P));
        result.pressure.push_back(P);
        result.massProfile.push_back(m);
    }

    return result;
}

std::vector<std::pair<double, double>> TOVSolver::massRadiusCurve(
    double rhoMin, double rhoMax, int samples) const {
    std::vector<std::pair<double, double>> curve;
    curve.reserve(samples);

    // Log-spaced central densities
    const double logMin = std::log10(rhoMin);
    const double logMax = std::log10(rhoMax);

    for (int i = 0; i < samples; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(samples - 1);
        const double rho = std::pow(10.0, logMin + t * (logMax - logMin));
        const TOVResult res = solve(rho);
        if (res.converged && !res.collapsed) {
            curve.emplace_back(res.mass, res.radius);
        }
    }

    return curve;
}

double TOVSolver::maximumMass(double rhoMin, double rhoMax, int samples) const {
    double maxMass = 0.0;
    for (const auto& [mass, radius] : massRadiusCurve(rhoMin, rhoMax, samples)) {
        maxMass = std::max(maxMass, mass);
    }
    return maxMass;
}

} // namespace nsim::physics