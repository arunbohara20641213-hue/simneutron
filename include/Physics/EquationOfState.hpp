#pragma once

namespace nsim::physics {

/**
 * Abstract equation of state: P = P(rho).
 * Provides pressure and energy density from baryon density.
 */
class EquationOfState {
public:
    virtual ~EquationOfState() = default;

    /**
     * Pressure as a function of density.
     * @param rho Mass density [kg/m^3]
     * @return Pressure [Pa = N/m^2]
     */
    virtual double pressure(double rho) const = 0;

    /**
     * Energy density (including rest mass) as a function of density.
     * @param rho Mass density [kg/m^3]
     * @return Energy density [J/m^3]
     */
    virtual double energyDensity(double rho) const = 0;

    /**
     * Inverse: density as a function of pressure.
     * @param P Pressure [Pa]
     * @return Mass density [kg/m^3]
     */
    virtual double pressureInverse(double P) const = 0;

    /**
     * Name of this EOS for display.
     */
    virtual const char* name() const = 0;
};

/**
 * Polytropic equation of state: P = K * rho^gamma
 * This is a simple, commonly used approximation for neutron star interiors.
 */
class PolytropicEOS : public EquationOfState {
public:
    /**
     * @param K Polytropic constant [SI units: m^5/(kg s^2) for gamma=2]
     * @param gamma Adiabatic index (dimensionless)
     */
    PolytropicEOS(double K, double gamma);

    double pressure(double rho) const override;
    double energyDensity(double rho) const override;
    double pressureInverse(double P) const override;
    const char* name() const override;

    double K() const { return K_; }
    double gamma() const { return gamma_; }

    void setK(double K) { K_ = K; }
    void setGamma(double gamma) { gamma_ = gamma; }

private:
    double K_;
    double gamma_;
};

} // namespace nsim::physics