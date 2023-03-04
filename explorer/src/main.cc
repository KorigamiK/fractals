#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Logging {

enum LogLevel { Error = 0, Info };

namespace detail {
bool debug = true;
}

void log(LogLevel level, const std::string& message) {
  if (level == LogLevel::Error) {
    std::cerr << "Error: " << message << std::endl;
  } else if (detail::debug) {
    std::cout << "Info: " << message << std::endl;
  }
}

}  // namespace Logging

std::string read_file(const std::string& filename) {
  std::ifstream file(filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    Logging::log(Logging::Error, "Could not initialize SDL");
    SDL_Quit();
    exit(1);
  }

  // Set SDL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Create a window
  SDL_Window* window = SDL_CreateWindow(
      "Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    Logging::log(Logging::Error, "Could not create SDL window");
    SDL_Quit();
    exit(1);
  }

  // Create a GL context
  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!context) {
    Logging::log(Logging::Error, "Could not create GL context");
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  // Initialize GLEW
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    Logging::log(Logging::Error, "Could not initialize GLEW");
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  // Set up the viewport
  glViewport(0, 0, 800, 600);

  // Load the initial vertex shader
  std::string vertexShaderCode = read_file("shaders/vertex_shader.glsl");
  const char* vertexShaderSource = vertexShaderCode.c_str();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLint success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    Logging::log(Logging::Error,
                 std::string("Vertex shader compilation failed: ") + infoLog);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  // Load the initial fragment shader
  std::string fragmentShaderCode = read_file("shaders/fragment_shader.glsl");
  const char* fragmentShaderSource = fragmentShaderCode.c_str();

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    Logging::log(Logging::Error,
                 std::string("Fragment shader compilation failed: ") + infoLog);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  // Create a shader program and attach the vertex and fragment shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    Logging::log(Logging::Error,
                 std::string("Shader program linking failed: ") + infoLog);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  // Delete the vertex and fragment shaders now that they're linked into the
  // program
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Create a VAO (Vertex Array Object) to store the vertex buffer bindings and
  // attribute config
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a VBO (Vertex Buffer Object) to store the vertex data
  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Configure the vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // Render loop
  bool quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }
    // Set the clear color to black and clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Bind the VAO and draw the triangle
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap buffers
    SDL_GL_SwapWindow(window);
  }

  // Clean up
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shaderProgram);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
