#define GLM_FORCE_PURE

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#else
#include <GL/glew.h>
#endif

#include <string>

#include "logger.hh"
#include "window.hh"

#define WIDTH 800
#define HEIGHT 600

auto main(int argc, const char* argv[]) -> int {
  (void)argc;
  (void)argv;

  Window window("The Mandlebrot", WIDTH, HEIGHT);

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

  auto mainLoop = [&window]() {
    window.update();
    window.clear();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    window.swapBuffers();
  };

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(
      [](void* arg) {
        auto loopFunc = reinterpret_cast<decltype(mainLoop)*>(arg);
        (*loopFunc)();
      },
      &mainLoop, -1, 1);
#else
  try {
    while (window.isRunning()) {
      mainLoop();
    }
  } catch (const std::exception& e) {
    Logger::error("Exception: %s", e.what());
  }

#endif
  Logger::info("Exiting");

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  return 0;
}
