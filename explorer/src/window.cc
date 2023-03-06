#include "window.hh"

#include <glm/glm.hpp>

Window::Window(const std::string& title,
               int width,
               int height,
               SceneData sceneData)
    : width(width),
      height(height),
      sceneData(sceneData),
      lastFrameTime(SDL_GetTicks()) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    Logger::error("Could not initialize SDL");
    SDL_Quit();
    exit(1);
  }

  // WebGL 2.0 == OpenGL ES 3.0
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height, flags);
  if (!window) {
    Logger::error("Could not create SDL window");
    SDL_Quit();
    exit(1);
  }

  glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    Logger::error("Could not create OpenGL context");
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  // Enable VSync
  SDL_GL_SetSwapInterval(1);

  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    Logger::error("Could not initialize GLEW");
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader = std::make_unique<Shader>("shaders/vertex_shader.glsl",
                                    "shaders/fragment_shader.glsl");

  glViewport(0, 0, width, height);
  shader->use();

  // Get uniform locations
  uniformLocations.zoom = shader->getUniformLocation("u_zoom");
  uniformLocations.center = shader->getUniformLocation("u_center");
  uniformLocations.maxIterations =
      shader->getUniformLocation("u_max_iterations");

  // Set initial uniform values
  glUniform1f(uniformLocations.zoom, sceneData.zoom);
  glUniform2f(uniformLocations.center, sceneData.center.x, sceneData.center.y);
  glUniform1i(uniformLocations.maxIterations, sceneData.maxIterations);
}

Window::~Window() {
  Logger::info("Window::~Window()");
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Window::clear() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::update() {
  pollEvents();
  if (shouldReloadShaders) {
    Logger::info("Reloading shaders");
    shader->reload();
    shouldReloadShaders = false;
    shader->use();
  }
}
void Window::pollEvents() {
  static SDL_Event event;

  static bool isDragging = false;
  static bool isPinching = false;
  static glm::vec2 dragStart(0.0f, 0.0f);
  static glm::vec2 pinchStart(0.0f, 0.0f);
  static float initialPinchDistance = 0.0f;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;

      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          glViewport(0, 0, event.window.data1, event.window.data2);
          width = event.window.data1;
          height = event.window.data2;
        }
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_r:
            shouldReloadShaders = true;
            break;
          case SDLK_ESCAPE:
            running = false;
            break;
          case SDLK_UP:
            sceneData.maxIterations++;
            glUniform1i(uniformLocations.maxIterations,
                        sceneData.maxIterations);
            break;
          case SDLK_DOWN:
            sceneData.maxIterations--;
            glUniform1i(uniformLocations.maxIterations,
                        sceneData.maxIterations);
            break;
          case SDLK_LEFT:
            sceneData.center.x -= 0.1f / sceneData.zoom;
            glUniform2f(uniformLocations.center, sceneData.center.x,
                        sceneData.center.y);
            break;
          case SDLK_RIGHT:
            sceneData.center.x += 0.1f / sceneData.zoom;
            glUniform2f(uniformLocations.center, sceneData.center.x,
                        sceneData.center.y);
            break;
          default:
            break;
        }
        break;

      case SDL_MOUSEWHEEL: {
        int x, y;
        SDL_GetMouseState(&x, &y);
        glm::vec2 cursor_pos =
            glm::vec2(x, y) / glm::vec2(width, height) * 2.0f - 1.0f;
        cursor_pos.y *= -1.0f;
        float new_zoom =
            sceneData.zoom * static_cast<float>(pow(0.9f, event.wheel.y));
        glm::vec2 center_offset =
            cursor_pos / sceneData.zoom - cursor_pos / new_zoom;
        sceneData.center += center_offset;
        sceneData.zoom = new_zoom;
        glUniform2f(uniformLocations.center, sceneData.center.x,
                    sceneData.center.y);
        glUniform1f(uniformLocations.zoom, sceneData.zoom);
      } break;

      case SDL_MOUSEBUTTONDOWN:
        switch (event.button.button) {
          case SDL_BUTTON_LEFT:
            isDragging = true;
            dragStart.x = static_cast<float>(event.button.x);
            dragStart.y = static_cast<float>(event.button.y);
            break;
          case SDL_BUTTON_RIGHT:
            isPinching = true;
            pinchStart.x = static_cast<float>(event.button.x);
            pinchStart.y = static_cast<float>(event.button.y);
            initialPinchDistance = 0.0f;
            break;
          default:
            break;
        }
        break;

      case SDL_MOUSEMOTION:
        if (isDragging) {
          glm::vec2 currentPos(event.motion.x, event.motion.y);
          glm::vec2 delta = (currentPos - dragStart) / glm::vec2(width, height);
          delta.y *= -1.0f;
          sceneData.center -= delta / sceneData.zoom;
          glUniform2f(uniformLocations.center, sceneData.center.x,
                      sceneData.center.y);
          dragStart = currentPos;
        } else if (isPinching && event.dgesture.numFingers >= 2) {
          glm::vec2 currentPos(event.motion.x, event.motion.y);
          glm::vec2 delta =
              (currentPos - pinchStart) / glm::vec2(width, height);
          delta.y *= -1.0f;
          float pinchDistance = glm::length(delta);
          if (initialPinchDistance == 0.0f) {
            initialPinchDistance = pinchDistance;
          } else {
            float new_zoom =
                sceneData.zoom * pinchDistance / initialPinchDistance;
            glm::vec2 cursor_pos = glm::vec2(event.motion.x, event.motion.y) /
                                       glm::vec2(width, height) * 2.0f -
                                   1.0f;
            cursor_pos.y *= -1.0f;
            glm::vec2 center_offset =
                cursor_pos / sceneData.zoom - cursor_pos / new_zoom;
            sceneData.center += center_offset;
            sceneData.zoom = new_zoom;
            glUniform2f(uniformLocations.center, sceneData.center.x,
                        sceneData.center.y);
            glUniform1f(uniformLocations.zoom, sceneData.zoom);
          }
        }
        break;

      case SDL_MOUSEBUTTONUP:
        switch (event.button.button) {
          case SDL_BUTTON_LEFT:
            isDragging = false;
            break;
          case SDL_BUTTON_RIGHT:
            isPinching = false;
            break;
          default:
            break;
        }
        break;

      default:
        break;
    }
  }
}

void Window::swapBuffers() {
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
