#pragma once

#include <memory>

#include "Rendering/Camera.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Shader.hpp"

#include "Simulation/Simulation.hpp"

namespace nsim::rendering {

class Renderer {
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init(int width, int height);
    void render(const nsim::simulation::Simulation& sim, const Camera& camera);
    void resize(int width, int height);

    void setStarMesh(std::unique_ptr<Mesh> mesh) { starMesh_ = std::move(mesh); }
    void setDiskMesh(std::unique_ptr<Mesh> mesh) { diskMesh_ = std::move(mesh); }
    void setParticleMesh(std::unique_ptr<Mesh> mesh) { particleMesh_ = std::move(mesh); }
    void setDefaultShader(std::unique_ptr<Shader> shader) { defaultShader_ = std::move(shader); }

private:
    std::unique_ptr<Framebuffer> framebuffer_;
    std::unique_ptr<Shader> defaultShader_;
    std::unique_ptr<Shader> diskShader_;
    std::unique_ptr<Mesh> starMesh_;
    std::unique_ptr<Mesh> diskMesh_;
    std::unique_ptr<Mesh> particleMesh_;
};

} // namespace nsim::rendering
