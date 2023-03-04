#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>

#include "logger.hh"
#include "shader.hh"
#include "window.hh"

int main() {
  // Initialize SDL and create a window
  Window window("Triangle", 800, 600);

  // Load the initial vertex shader and fragment shader
  std::string vertexShaderFile = "shaders/vertex_shader.glsl";
  std::string fragmentShaderFile = "shaders/fragment_shader.glsl";
  Shader shader(vertexShaderFile.c_str(), fragmentShaderFile.c_str());

  // Create a VAO (Vertex Array Object) and VBO (Vertex Buffer Object) to store
  // the vertex data
  float vertices[] = {-0.5f, -0.5f, 0.0f,   // top left
                      0.5f,  -0.5f, 0.0f,   // top right
                      0.0f,  0.5f,  0.0f};  // bottom
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // Set up the viewport
  glViewport(0, 0, window.getWidth(), window.getHeight());

  try {
    // Main loop
    while (window.isRunning()) {
      // Handle events
      window.pollEvents();

      if (window.shouldReloadShader()) {
        Logger::info("Reloading shaders");
        shader.reload();
      }

      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Draw the triangle
      shader.use();
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      // Swap buffers
      window.swapBuffers();
    }
  } catch (const std::exception& e) {
    Logger::error(e.what());
  }

  return 0;
}
