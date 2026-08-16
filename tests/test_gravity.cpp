#include <cassert>
#include <cmath>

#include <spdlog/spdlog.h>

#include "Physics/Constants.hpp"
#include "Physics/Gravity.hpp"

using namespace nsim::physics;

void test_acceleration_magnitude() {
    // Test: g = GM/r^2
    // For a 1 kg mass at r = 1 m: g = 6.67430e-11 m/s^2
    double g = gravity::accelerationMagnitude(1.0, 1.0);
    double expected = G;
    assert(std::abs(g - expected) < 1e-15);
    spdlog::info("test_acceleration_magnitude PASSED");
}

void test_escape_velocity() {
    // Test: v_esc = sqrt(2GM/r)
    // For Earth mass at surface: v_esc ≈ 11186 m/s
    double mass = 5.972e24;  // Earth mass [kg]
    double r = 6.371e6;      // Earth radius [m]
    double v = gravity::escapeVelocity(mass, r);
    double expected = std::sqrt(2.0 * G * mass / r);
    assert(std::abs(v - expected) < 1.0);
    spdlog::info("test_escape_velocity PASSED");
}

void test_schwarzschild_radius() {
    // Test: r_s = 2GM/c^2
    // For Sun: r_s ≈ 2953 m
    double mass = M_sun;
    double r = gravity::schwarzschildRadius(mass);
    double expected = 2.0 * G * M_sun / (c * c);
    assert(std::abs(r - expected) < 1.0);
    spdlog::info("test_schwarzschild_radius PASSED");
}

void test_compactness() {
    // Test: C = 2GM/(Rc^2)
    // For a 1.4 solar mass, 12 km radius neutron star:
    double mass = DEFAULT_STAR_MASS;
    double radius = DEFAULT_STAR_RADIUS;
    double cpt = gravity::compactness(mass, radius);
    // Should be less than 1 for a normal neutron star (R > r_s)
    assert(cpt < 1.0);
    spdlog::info("test_compactness PASSED (compactness = {:.4f})", cpt);
}

void test_potential() {
    // Test: Phi = -GM/r
    double mass = 1.0;
    double r = 1.0;
    double phi = gravity::potential(mass, r);
    double expected = -G * mass / r;
    assert(std::abs(phi - expected) < 1e-15);
    spdlog::info("test_potential PASSED");
}

int main() {
    test_acceleration_magnitude();
    test_escape_velocity();
    test_schwarzschild_radius();
    test_compactness();
    test_potential();
    spdlog::info("All gravity tests PASSED");
    return 0;
}