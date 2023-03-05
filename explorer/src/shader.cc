#include "shader.hh"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include "logger.hh"

Shader::Shader(const char* vertexShaderFile, const char* fragmentShaderFile)
    : m_vertexShaderFile(vertexShaderFile),
      m_fragmentShaderFile(fragmentShaderFile) {
  auto vertexShader = compileShader(GL_VERTEX_SHADER, m_vertexShaderFile);
  auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, m_fragmentShaderFile);

  m_program = linkShaderProgram(vertexShader, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::~Shader() {
  glDeleteProgram(m_program);
}

void Shader::reload() {
  auto vertexShader = compileShader(GL_VERTEX_SHADER, m_vertexShaderFile);
  auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, m_fragmentShaderFile);

  auto program = linkShaderProgram(vertexShader, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glDeleteProgram(m_program);
  m_program = program;
}

GLuint Shader::compileShader(GLenum shaderType, const char* source) {
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

void Shader::use() const {
  glUseProgram(m_program);
}

void Shader::setBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(m_program, name.c_str()),
              static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

GLint Shader::getUniformLocation(const std::string& name) const {
  return glGetUniformLocation(m_program, name.c_str());
}

GLuint Shader::compileShader(GLenum type, const char* source) const {
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

std::string Shader::read_file(const std::string& filepath) const {
  std::ifstream file(filepath);
  if (!file) {
    Logger::error("Failed to open file: %s", filepath.c_str());
    throw std::runtime_error("Failed to open file");
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

GLuint Shader::linkShaderProgram(GLuint vertexShader,
                                 GLuint fragmentShader) const {
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