#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>

#include "logger.hh"
#include "shader.hh"

class Window {
 public:
  Window(const std::string& title, int width, int height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      Logger::error("Could not initialize SDL");
      SDL_Quit();
      exit(1);
    }

    // Set SDL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create an SDL window
    SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (!window) {
      Logger::error("Could not create SDL window");
      SDL_Quit();
      exit(1);
    }

    // Create an OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
      Logger::error("Could not create OpenGL context");
      SDL_DestroyWindow(window);
      SDL_Quit();
      exit(1);
    }

    // Enable VSync
    // SDL_GL_SetSwapInterval(1);

    // Initialize GLEW
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
      Logger::error("Could not initialize GLEW");
      SDL_GL_DeleteContext(glContext);
      SDL_DestroyWindow(window);
      SDL_Quit();
      exit(1);
    }

    // Set up OpenGL options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up variables
    running = true;
    lastFrameTime = SDL_GetTicks();
    frameCount = 0;
    fps = 0;
    shouldReloadShaders = false;
  }

  ~Window() {
    // Clean up SDL and OpenGL resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            glViewport(0, 0, event.window.data1, event.window.data2);
          break;

        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_r)
            shouldReloadShaders = true;
          break;

        default:
          break;
      }
    }
  }

  void swapBuffers() {
    SDL_GL_SwapWindow(window);
    frameCount++;
    Uint32 currentFrameTime = SDL_GetTicks();
    if (currentFrameTime - lastFrameTime >= 1000) {
      fps = frameCount;
      frameCount = 0;
      lastFrameTime = currentFrameTime;
      Logger::info("FPS: %d", fps);
    }
  }

  int getWidth() const { return width; }

  int getHeight() const { return height; }

  bool isRunning() const { return running; }

  bool shouldReloadShader() {
    bool result = shouldReloadShaders;
    shouldReloadShaders = false;
    return result;
  }

 private:
  SDL_Window* window;
  SDL_GLContext glContext;
  bool running;
  int width, height;
  Uint32 lastFrameTime;
  int frameCount, fps;
  bool shouldReloadShaders;
};
