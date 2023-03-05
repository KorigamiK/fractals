#include "window.hh"

Window::Window(const std::string& title,
               int width,
               int height,
               SceneData sceneData)
    : width(width),
      height(height),
      running(true),
      frameCount(0),
      fps(0),
      sceneData(sceneData),
      shouldReloadShaders(false),
      lastFrameTime(SDL_GetTicks()) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    Logger::error("Could not initialize SDL");
    SDL_Quit();
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
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

  // Set up the viewport
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

      case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0) {
          sceneData.zoom *= 0.9f;
        } else if (event.wheel.y < 0) {
          sceneData.zoom *= 1.1f;
        }
        glUniform1f(uniformLocations.zoom, sceneData.zoom);
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