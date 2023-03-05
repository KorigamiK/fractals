#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "logger.hh"
#include "window.hh"

#define WIDTH 480
#define HEIGHT 360

int main() {
  Window window("Triangle", WIDTH, HEIGHT);

  // /*
  // set up vertex buffer object and vertex array object
  GLfloat vertices[] = {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f};
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  // */

  try {
    while (window.isRunning()) {
      window.update();
      window.clear();

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      window.swapBuffers();
    }
  } catch (const std::exception& e) {
    Logger::error(e.what());
  }

  Logger::info("Exiting");

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  return 0;
}
