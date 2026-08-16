#include <cassert>
#include <cmath>

#include <spdlog/spdlog.h>

#include "Physics/Constants.hpp"
#include "Physics/Relativity.hpp"
#include "Physics/Gravity.hpp"

using namespace nsim::physics;

void test_time_dilation() {
    // Test: dtau/dt = sqrt(1 - 2GM/(rc^2))
    // At infinity: time dilation = 1
    double dilation = relativity::timeDilation(M_sun, 1e15);  // Far enough for < 1e-10 deviation
    assert(std::abs(dilation - 1.0) < 1e-10);
    spdlog::info("test_time_dilation at infinity PASSED");

    // At Schwarzschild radius: time dilation = 0
    double rs = gravity::schwarzschildRadius(M_sun);
    double dilationAtHorizon = relativity::timeDilation(M_sun, rs);
    assert(dilationAtHorizon < 1e-10);
    spdlog::info("test_time_dilation at horizon PASSED");
}

void test_gravitational_redshift() {
    // Test: z = 1/sqrt(1 - 2GM/(rc^2)) - 1
    // At infinity: redshift = 0
    double rs = gravity::schwarzschildRadius(M_sun);
    double z = relativity::gravitationalRedshift(M_sun, 1e15);
    assert(std::abs(z) < 1e-10);
    spdlog::info("test_gravitational_redshift at infinity PASSED");

    // Near Schwarzschild radius: redshift diverges.
    // At r = 1.01 * r_s, z = 1/sqrt(1 - 1/1.01) - 1 ~= 9.06.
    double zAtHorizon = relativity::gravitationalRedshift(M_sun, 1.01 * rs);
    assert(std::isinf(zAtHorizon) || zAtHorizon > 5.0);
    spdlog::info("test_gravitational_redshift at horizon PASSED");
}

void test_schwarzschild_radius() {
    // Test Schwarzschild radius calculation
    double mass = M_sun;
    double rs = gravity::schwarzschildRadius(mass);
    double expected = 2.0 * G * M_sun / (c * c);
    assert(std::abs(rs - expected) < 1.0);
    spdlog::info("test_schwarzschild_radius PASSED (r_s = {:.1f} m)", rs);
}

void test_escape_velocity() {
    // Test escape velocity
    double mass = M_sun;
    double r = 1e9;  // 1 million km
    double v = gravity::escapeVelocity(mass, r);
    double expected = std::sqrt(2.0 * G * mass / r);
    assert(std::abs(v - expected) < 1.0);
    spdlog::info("test_escape_velocity PASSED");
}

int main() {
    test_time_dilation();
    test_gravitational_redshift();
    test_schwarzschild_radius();
    test_escape_velocity();
    spdlog::info("All relativity tests PASSED");
    return 0;
}