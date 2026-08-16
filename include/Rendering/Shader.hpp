#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>

namespace nsim::rendering {

/**
 * OpenGL shader program wrapper.
 * Compiles vertex/fragment shaders and links them into a program.
 */
class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    /**
     * Load and compile shaders from source strings.
     * @param vertexSource GLSL vertex shader source
     * @param fragmentSource GLSL fragment shader source
     * @throws std::runtime_error on compilation/link failure
     */
    void loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * Load shaders from files.
     * @param vertexPath Path to vertex shader file
     * @param fragmentPath Path to fragment shader file
     * @throws std::runtime_error on file read or compilation failure
     */
    void loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * Use this shader program.
     */
    void use() const;

    /**
     * Uniform setters.
     */
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    /**
     * OpenGL program ID.
     */
    unsigned int id() const { return id_; }

private:
    unsigned int id_ = 0;
    bool valid_ = false;

    static unsigned int compileShader(unsigned int type, const std::string& source);
    static std::string readFile(const std::string& path);
};

} // namespace nsim::rendering