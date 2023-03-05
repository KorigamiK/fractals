#pragma once

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>

#include "logger.hh"

class Shader {
 public:
  Shader(const char* vertexShaderFile, const char* fragmentShaderFile)
      : m_vertexShaderFile(vertexShaderFile),
        m_fragmentShaderFile(fragmentShaderFile) {
    auto vertexShader = compileShader(GL_VERTEX_SHADER, m_vertexShaderFile);
    auto fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, m_fragmentShaderFile);

    m_program = linkShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  ~Shader() { glDeleteProgram(m_program); }

  void reload() {
    auto vertexShader = compileShader(GL_VERTEX_SHADER, m_vertexShaderFile);
    auto fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, m_fragmentShaderFile);

    auto program = linkShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glDeleteProgram(m_program);
    m_program = program;
  }

  GLuint compileShader(GLenum shaderType, const char* source) {
    std::string vertexShaderSource = read_file(source);
    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();

    GLuint vertexShader = glCreateShader(shaderType);
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      GLchar infoLog[512];
      glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
      Logger::error("Shader compilation failed: %s", infoLog);
      throw std::runtime_error("Shader compilation failed");
    }

    return vertexShader;
  }

  void use() const { glUseProgram(m_program); }

  void setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_program, name.c_str()),
                static_cast<int>(value));
  }

  void setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
  }

  void setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
  }

  GLuint getProgram() const { return m_program; }

  GLint getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(m_program, name.c_str());
  }

 private:
  GLuint m_program;

  const char* m_vertexShaderFile = "shaders/vertex_shader.glsl";
  const char* m_fragmentShaderFile = "shaders/fragment_shader.glsl";

  GLuint compileShader(GLenum type, const char* source) const {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      GLchar infoLog[512];
      glGetShaderInfoLog(shader, 512, nullptr, infoLog);
      Logger::error("Shader compilation failed: %s", infoLog);
      throw std::runtime_error("Shader compilation failed");
    }

    return shader;
  }

  GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) const {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
      GLchar infoLog[512];
      glGetProgramInfoLog(program, 512, nullptr, infoLog);
      Logger::error("Shader linking failed: %s", infoLog);
      throw std::runtime_error("Shader linking failed");
    }

    return program;
  }

  std::string read_file(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file) {
      Logger::error("Failed to open file: %s", filepath.c_str());
      throw std::runtime_error("Failed to open file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
};
