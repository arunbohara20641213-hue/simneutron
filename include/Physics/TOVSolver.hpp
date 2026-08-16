#pragma once

#include "Physics/EquationOfState.hpp"

#include <utility>
#include <vector>

namespace nsim::physics {

/**
 * Result of a TOV integration.
 */
struct TOVResult {
    double radius = 0.0;          // Surface radius [m]
    double mass = 0.0;            // Total mass [kg]
    double centralDensity = 0.0;  // Central density [kg/m^3]
    double centralPressure = 0.0; // Central pressure [Pa]
    double surfacePressure = 0.0; // Pressure at surface [Pa]
    double compactness = 0.0;     // 2GM/(Rc^2)
    bool converged = false;       // Whether integration reached zero pressure
    bool collapsed = false;       // Whether star collapsed to a black hole (R <= r_s)

    // Radial profiles
    std::vector<double> r;        // Radius [m]
    std::vector<double> density;  // Density [kg/m^3]
    std::vector<double> pressure; // Pressure [Pa]
    std::vector<double> massProfile; // Enclosed mass [kg]
};

/**
 * Numerical Tolman-Oppenheimer-Volkoff solver.
 *
 * Integrates the TOV equations:
 *   dm/dr = 4*pi*r^2*rho
 *   dP/dr = -G*(rho + P/c^2)*(m + 4*pi*r^3*P/c^2) / (r^2*(1 - 2Gm/(rc^2)))
 *
 * Starting from a central density, integrating outward until pressure
 * approaches zero. The result is the radius, mass, and internal profiles.
 */
class TOVSolver {
public:
    /**
     * @param eos Equation of state to use
     * @param dr Radial step size [m]
     */
    explicit TOVSolver(const EquationOfState& eos, double dr = 10.0);

    /**
     * Solve the TOV equations for a given central density.
     * @param centralDensity Central density [kg/m^3]
     * @return TOV result with profiles
     */
    TOVResult solve(double centralDensity) const;

    /**
     * Generate a mass-radius curve over a range of central densities.
     * @param rhoMin Minimum central density [kg/m^3]
     * @param rhoMax Maximum central density [kg/m^3]
     * @param samples Number of samples
     * @return Vector of (mass [kg], radius [m]) pairs
     */
    std::vector<std::pair<double, double>> massRadiusCurve(
        double rhoMin, double rhoMax, int samples) const;

    /**
     * Find the approximate maximum mass supported by the EOS.
     * @param rhoMin Minimum central density [kg/m^3]
     * @param rhoMax Maximum central density [kg/m^3]
     * @param samples Number of samples
     * @return Maximum mass [kg]
     */
    double maximumMass(double rhoMin, double rhoMax, int samples) const;

private:
    const EquationOfState& eos_;
    double dr_;
};

} // namespace nsim::physics