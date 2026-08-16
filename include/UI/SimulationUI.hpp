#pragma once

#include <imgui.h>

namespace nsim::simulation {
class Simulation;
}

namespace nsim::ui {

/**
 * Dear ImGui interface for the neutron star simulator.
 */
class SimulationUI {
public:
    SimulationUI();

    /**
     * Render the ImGui UI panels.
     * @param simulation Reference to the simulation
     */
    void render(nsim::simulation::Simulation& simulation);

    /**
     * Update simulation parameters from UI.
     * @param simulation Reference to the simulation
     */
    void updateParameters(nsim::simulation::Simulation& simulation);

private:
    // SIMULATION panel
    bool showSimulationPanel_ = true;
    bool showNeutronStarPanel_ = true;
    bool showPhysicsPanel_ = true;
    bool showTOVPanel_ = true;
    bool showVisualizationPanel_ = true;
    bool showStatisticsPanel_ = true;

    // Scientific disclaimer
    bool showDisclaimer_ = true;
};

} // namespace nsim::ui