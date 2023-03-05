#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <string>

#include "logger.hh"
#include "shader.hh"

struct SceneData {
  float zoom;
  glm::vec2 center;
  int maxIterations;
};

struct UniformLocations {
  GLint zoom;
  GLint center;
  GLint maxIterations;
};

class Window {
 private:
  int width, height;
  bool running;
  int frameCount, fps;
  SceneData sceneData;
  bool shouldReloadShaders;
  Uint32 lastFrameTime;
  SDL_Window* window;
  SDL_GLContext glContext;
  std::unique_ptr<Shader> shader;
  UniformLocations uniformLocations;

 public:
  Window(const std::string& title,
         int width,
         int height,
         SceneData sceneData = {
             .zoom = 1.0f,
             .center = glm::vec2(0.0f, 0.0f),
             .maxIterations = 100,
         });

  ~Window();

  SceneData& getSceneData() { return sceneData; }

  void clear();

  void update();

  void pollEvents();

  void swapBuffers();

  int getWidth() const { return width; }

  int getHeight() const { return height; }

  bool isRunning() const { return running; }
};
