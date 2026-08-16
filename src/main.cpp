#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include "Rendering/Camera.hpp"
#include "Rendering/Renderer.hpp"
#include "Simulation/Simulation.hpp"
#include "UI/SimulationUI.hpp"

#include <glm/glm.hpp>

// Forward-declared pointer to the renderer, used by the resize callback.
static nsim::rendering::Renderer* gRenderer = nullptr;

// Callback for window resize
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
    if (gRenderer) {
        gRenderer->resize(width, height);
    }
}

// Callback for mouse movement (orbit / pan)
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* camera = static_cast<nsim::rendering::Camera*>(glfwGetWindowUserPointer(window));
    if (!camera) return;

    static double lastX = xpos;
    static double lastY = ypos;
    static bool first = true;

    if (first) {
        lastX = xpos;
        lastY = ypos;
        first = false;
        return;
    }

    const double dx = xpos - lastX;
    const double dy = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera->orbit(static_cast<float>(dx) * 0.005f, static_cast<float>(dy) * 0.005f);
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        camera->pan(static_cast<float>(dx) * 0.01f, static_cast<float>(dy) * 0.01f);
    }
}

// Callback for scroll (zoom)
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;
    auto* camera = static_cast<nsim::rendering::Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->zoom(static_cast<float>(yoffset));
    }
}

int main() {
    spdlog::info("Neutron Star Simulator starting...");

    // Initialize GLFW
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Neutron Star Simulator", nullptr, nullptr);
    if (!window) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Load OpenGL functions via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

    // Camera
    nsim::rendering::Camera camera{};
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Renderer
    nsim::rendering::Renderer renderer{};
    gRenderer = &renderer;
    int displayW, displayH;
    glfwGetFramebufferSize(window, &displayW, &displayH);
    renderer.init(displayW, displayH);

    // Simulation
    nsim::simulation::Simulation simulation{};
    nsim::ui::SimulationUI ui{};

    // Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Enable vsync
    glfwSwapInterval(1);

    // Input state
    bool spacePressed = false;
    bool rPressed = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Quit
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Pause / resume (Space)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            simulation.params().paused = !simulation.params().paused;
            spacePressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }

        // Reset camera (R)
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rPressed) {
            camera.reset();
            rPressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
            rPressed = false;
        }

        // Toggle camera mode (M)
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            camera.toggleMode();
        }

        // Simulation update
        const float dt = 1.0f / 60.0f;
        simulation.step(dt);

        // Camera movement (WASD)
        const float forward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ? 1.0f : 0.0f)
                            - (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? 1.0f : 0.0f);
        const float right = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? 1.0f : 0.0f)
                          - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? 1.0f : 0.0f);
        const float up = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ? 1.0f : 0.0f)
                       - (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ? 1.0f : 0.0f);
        camera.move(forward, right, up, dt);

        // Clear the default framebuffer to prevent ghost trails from ImGui
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene
        renderer.render(simulation, camera);

        // Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render UI panels
        ui.render(simulation);
        ui.updateParameters(simulation);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}