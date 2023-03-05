#pragma once

#include <GL/glew.h>
#include <string>

class Shader {
 public:
  Shader(const char* vertexShaderFile, const char* fragmentShaderFile);

  ~Shader();

  void reload();

  GLuint compileShader(GLenum shaderType, const char* source);

  void use() const;

  void setBool(const std::string& name, bool value) const;

  void setInt(const std::string& name, int value) const;

  void setFloat(const std::string& name, float value) const;

  inline GLuint getProgram() const { return m_program; }

  GLint getUniformLocation(const std::string& name) const;

 private:
  GLuint m_program;

  const char* m_vertexShaderFile;
  const char* m_fragmentShaderFile;

  GLuint compileShader(GLenum type, const char* source) const;

  GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) const;

  std::string read_file(const std::string& filepath) const;
};
