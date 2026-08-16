#include "UI/SimulationUI.hpp"

#include "Simulation/Simulation.hpp"

#include <imgui.h>

namespace nsim::ui {

SimulationUI::SimulationUI() = default;

void SimulationUI::render(nsim::simulation::Simulation& simulation) {
    if (showDisclaimer_) {
        ImGui::Begin("Scientific Disclaimer", &showDisclaimer_);
        ImGui::TextWrapped(
            "This simulation is an educational visualization of neutron-star dynamics and "
            "does not represent a validated physical model for production research."
        );
        ImGui::End();
    }

    if (showSimulationPanel_) {
        ImGui::Begin("Simulation", &showSimulationPanel_);
        ImGui::Checkbox("Paused", &simulation.params().paused);
        {
            const double min = 0.1; const double max = 10.0;
            ImGui::SliderScalar("Time Scale", ImGuiDataType_Double, &simulation.params().timeScale, &min, &max, "%.2f");
        }
        {
            const double min = 0.001; const double max = 0.05;
            ImGui::SliderScalar("Timestep", ImGuiDataType_Double, &simulation.params().timestep, &min, &max, "%.4f");
        }
        ImGui::SliderInt("Sub-Steps", &simulation.params().subSteps, 1, 20);
        ImGui::End();
    }

    if (showNeutronStarPanel_) {
        ImGui::Begin("Neutron Star", &showNeutronStarPanel_);
        ImGui::Text("Mass: %.3e kg", simulation.star().mass());
        ImGui::Text("Radius: %.3e m", simulation.star().radius());
        ImGui::End();
    }

    if (showPhysicsPanel_) {
        ImGui::Begin("Physics", &showPhysicsPanel_);
        ImGui::Checkbox("Newtonian Gravity", &simulation.params().newtonianGravity);
        ImGui::Checkbox("Relativistic Effects", &simulation.params().relativisticEffects);
        ImGui::Checkbox("Photon Rays", &simulation.params().photonRays);
        ImGui::End();
    }

    if (showTOVPanel_) {
        ImGui::Begin("TOV", &showTOVPanel_);
        ImGui::Text("Simulation Time: %.3e s", simulation.simulationTime());
        ImGui::Text("Unstable: %s", simulation.isUnstable() ? "yes" : "no");
        ImGui::End();
    }

    if (showVisualizationPanel_) {
        ImGui::Begin("Visualization", &showVisualizationPanel_);
        ImGui::Checkbox("Accretion Disk", &simulation.params().accretionDisk);
        ImGui::SliderInt("Disk Particles", &simulation.params().diskParticleCount, 100, 20000);
        {
            const double min = 1.0e3; const double max = 1.0e5;
            ImGui::SliderScalar("Disk Inner Radius", ImGuiDataType_Double, &simulation.params().diskInnerRadius, &min, &max, "%.1e");
        }
        {
            const double min = 1.0e4; const double max = 1.0e6;
            ImGui::SliderScalar("Disk Outer Radius", ImGuiDataType_Double, &simulation.params().diskOuterRadius, &min, &max, "%.1e");
        }
        ImGui::End();
    }

    if (showStatisticsPanel_) {
        ImGui::Begin("Statistics", &showStatisticsPanel_);
        ImGui::Text("Particles: %zu", simulation.particles().particles().size());
        ImGui::Text("Total Energy: %.3e J", simulation.totalEnergy());
        ImGui::End();
    }
}

void SimulationUI::updateParameters(nsim::simulation::Simulation& simulation) {
    (void)simulation;
}

} // namespace nsim::ui
