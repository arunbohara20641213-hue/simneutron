#include "Rendering/Shader.hpp"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nsim::rendering {

Shader::~Shader() {
    if (id_ != 0) {
        glDeleteProgram(id_);
    }
}

Shader::Shader(Shader&& other) noexcept
    : id_(other.id_), valid_(other.valid_) {
    other.id_ = 0;
    other.valid_ = false;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (id_ != 0) {
            glDeleteProgram(id_);
        }
        id_ = other.id_;
        valid_ = other.valid_;
        other.id_ = 0;
        other.valid_ = false;
    }
    return *this;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
    const unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        const char* typeName = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        spdlog::error("Shader compilation failed ({0}): {1}", typeName, infoLog);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader compilation failed: ") + infoLog);
    }
    return shader;
}

void Shader::loadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    if (id_ != 0) {
        glDeleteProgram(id_);
        id_ = 0;
    }

    const unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    id_ = glCreateProgram();
    glAttachShader(id_, vertexShader);
    glAttachShader(id_, fragmentShader);
    glLinkProgram(id_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int success = 0;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(id_, sizeof(infoLog), nullptr, infoLog);
        spdlog::error("Shader program link failed: {0}", infoLog);
        glDeleteProgram(id_);
        id_ = 0;
        throw std::runtime_error(std::string("Shader program link failed: ") + infoLog);
    }

    valid_ = true;
}

void Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    loadFromSource(readFile(vertexPath), readFile(fragmentPath));
}

void Shader::use() const {
    if (valid_) {
        glUseProgram(id_);
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    if (!valid_) return;
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    if (!valid_) return;
    glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    if (!valid_) return;
    glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void Shader::setFloat(const std::string& name, float value) const {
    if (!valid_) return;
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const {
    if (!valid_) return;
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

} // namespace nsim::rendering