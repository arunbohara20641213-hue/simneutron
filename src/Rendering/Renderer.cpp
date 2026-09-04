#include "Rendering/Renderer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Framebuffer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <stdexcept>
#include <vector>

namespace nsim::rendering {

using nsim::simulation::Simulation;

namespace {
// Simulation scale: 1 world unit = 1 km (see Physics/Constants.hpp).
constexpr float M_TO_WORLD = 0.001f;
}

Renderer::Renderer() = default;

Renderer::~Renderer() {
    if (framebuffer_) {
        framebuffer_->destroy();
    }
}

void Renderer::init(int width, int height) {
    // Initialize framebuffer for HDR
    framebuffer_ = std::make_unique<Framebuffer>();
    framebuffer_->create(width, height);

    // Look for shaders relative to the working directory (assets are copied
    // to the build directory by CMake), then relative to the executable's own
    // directory so the app works regardless of where it is launched from.
    std::vector<std::filesystem::path> assetCandidates;
    assetCandidates.emplace_back("assets/shaders");
    assetCandidates.emplace_back("../assets/shaders");
    assetCandidates.emplace_back("../../assets/shaders");

    const std::filesystem::path exeDir =
        std::filesystem::current_path();
    assetCandidates.emplace_back(exeDir / "assets" / "shaders");
    assetCandidates.emplace_back(exeDir / ".." / "assets" / "shaders");

    std::filesystem::path assetDir;
    for (const auto& candidate : assetCandidates) {
        if (std::filesystem::exists(candidate / "star.vert")) {
            assetDir = candidate;
            break;
        }
    }
    if (assetDir.empty()) {
        throw std::runtime_error("Could not locate shader assets directory");
    }

    // Star shader
    defaultShader_ = std::make_unique<Shader>();
    defaultShader_->loadFromFile(
        (assetDir / "star.vert").string(),
        (assetDir / "star.frag").string());

    // Disk shader
    diskShader_ = std::make_unique<Shader>();
    diskShader_->loadFromFile(
        (assetDir / "disk.vert").string(),
        (assetDir / "disk.frag").string());

    // Star mesh: unit sphere, scaled by the model matrix to the star radius.
    starMesh_ = std::make_unique<Mesh>();
    starMesh_->createSphere(1.0f, 48, 24);
    starMesh_->upload();

    // Disk mesh: annular ring in the XZ plane.
    diskMesh_ = std::make_unique<Mesh>();
    diskMesh_->createDisk(2.0f, 100.0f, 96);
    diskMesh_->upload();

    spdlog::info("Renderer initialized ({0}x{1})", width, height);
}

void Renderer::render(const Simulation& sim, const Camera& camera) {
    if (!framebuffer_) return;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    const float aspect = static_cast<float>(framebuffer_->width())
                       / static_cast<float>(framebuffer_->height());
    const glm::mat4 projection = camera.projectionMatrix(aspect);
    const glm::mat4 view = camera.viewMatrix();

    // Render to offscreen framebuffer
    framebuffer_->bind();
    glViewport(0, 0, framebuffer_->width(), framebuffer_->height());

    glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 lightPos = camera.position();

    // Render star
    if (starMesh_ && defaultShader_) {
        const float starRadiusWorld =
            static_cast<float>(sim.star().radius()) * M_TO_WORLD;
        const glm::mat4 model = glm::scale(
            glm::mat4(1.0f), glm::vec3(starRadiusWorld));

        defaultShader_->use();
        defaultShader_->setMat4("model", model);
        defaultShader_->setMat4("view", view);
        defaultShader_->setMat4("projection", projection);
        defaultShader_->setVec3("lightPos", lightPos);
        defaultShader_->setVec3("starColor", glm::vec3(0.85f, 0.7f, 0.45f));
        defaultShader_->setFloat("starRadius", starRadiusWorld);
        defaultShader_->setFloat("camRadius", camera.radius());
        starMesh_->draw();
    }

    // Render accretion disk
    if (diskMesh_ && diskShader_ && sim.disk().enabled()) {
        const float innerWorld =
            static_cast<float>(sim.params().diskInnerRadius) * M_TO_WORLD;
        const float outerWorld =
            static_cast<float>(sim.params().diskOuterRadius) * M_TO_WORLD;

        diskShader_->use();
        diskShader_->setMat4("model", glm::mat4(1.0f));
        diskShader_->setMat4("view", view);
        diskShader_->setMat4("projection", projection);
        diskShader_->setVec3("lightPos", lightPos);
        diskShader_->setFloat("diskInnerRadius", innerWorld);
        diskShader_->setFloat("diskOuterRadius", outerWorld);
        diskShader_->setVec3("diskColor", glm::vec3(1.0f, 0.55f, 0.3f));
        diskMesh_->draw();
    }

    // Present the HDR framebuffer to the default framebuffer.
    framebuffer_->unbind();

    const int fbW = framebuffer_->width();
    const int fbH = framebuffer_->height();

    int windowW = 0, windowH = 0;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) {
        windowW = fbW;
        windowH = fbH;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_->id());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fbW, fbH, 0, 0, windowW, windowH,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void Renderer::resize(int width, int height) {
    if (framebuffer_) {
        framebuffer_->resize(width, height);
    }
}

} // namespace nsim::rendering