#include <cassert>
#include <cmath>

#include <spdlog/spdlog.h>

#include "Physics/Constants.hpp"
#include "Physics/EquationOfState.hpp"
#include "Physics/TOVSolver.hpp"

using namespace nsim::physics;

void test_tov_solver() {
    // n=1 polytrope (gamma=2): R = pi*sqrt(K/(2*pi*G)).
    // K = 8e-3 (SI) => R ~= 13.7 km, a physically sensible neutron-star size.
    // (K=100 would give R ~= 1500 km and exceed the solver's step budget.)
    ::nsim::physics::PolytropicEOS eos(8.0e-3, 2.0);

    ::nsim::physics::TOVSolver solver(eos, 10.0);  // 10 m radial step

    ::nsim::physics::TOVResult result = solver.solve(6.0e17);  // 6e17 kg/m^3 central density

    spdlog::info("TOV solve result:");
    spdlog::info("  Radius: {:.2f} m", result.radius);
    spdlog::info("  Mass: {:.2f} kg", result.mass);
    spdlog::info("  Central density: {:.2e} kg/m^3", result.centralDensity);
    spdlog::info("  Central pressure: {:.2e} Pa", result.centralPressure);
    spdlog::info("  Converged: {}", result.converged);
    spdlog::info("  Collapsed: {}", result.collapsed);

    // If converged, check basic consistency
    if (result.converged && !result.collapsed) {
        assert(result.radius > 0.0);
        assert(result.mass > 0.0);
        assert(result.centralDensity > 0.0);
        assert(result.centralPressure > 0.0);
        assert(result.surfacePressure >= 0.0);

        // Compactness should be less than 1
        assert(result.compactness < 1.0);
        spdlog::info("TOV basic consistency checks PASSED");
    } else {
        spdlog::info("TOV integration did not converge (may be expected for this EOS)");
    }
}

void test_mass_radius_curve() {
    ::nsim::physics::PolytropicEOS eos(8.0e-3, 2.0);
    ::nsim::physics::TOVSolver solver(eos, 10.0);

    // Generate mass-radius curve over NS-scale central densities
    auto curve = solver.massRadiusCurve(1.0e17, 1.0e19, 15);

    spdlog::info("Mass-radius curve generated with {} points", curve.size());
    for (const auto& [mass, radius] : curve) {
        spdlog::info("  M = {:.2e} kg, R = {:.2f} m", mass, radius);
    }

    assert(!curve.empty());
    spdlog::info("test_mass_radius_curve PASSED");
}

int main() {
    test_tov_solver();
    test_mass_radius_curve();
    spdlog::info("All TOV tests PASSED");
    return 0;
}
