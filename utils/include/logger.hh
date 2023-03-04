#pragma once

#include <SDL2/SDL.h>

#include <iostream>
#include <string>

/** Logger namespace.
 * Usage:
 * @code
 * ...
 *     Logger::error("Shader compilation failed: %s", infoLog);
 *     Logger::warn("Shader compilation failed: %s", infoLog);
 *     Logger::info("Shader compilation failed: %s", infoLog);
 * ...
 * @endcode
 */
namespace Logger {

enum class Level {
  Error = SDL_LOG_PRIORITY_ERROR,
  Warn = SDL_LOG_PRIORITY_WARN,
  Info = SDL_LOG_PRIORITY_INFO,
};

/** Log a formatted message.
 * @param level The level of the message.
 * @param format The format of the message.
 * @param ... The arguments to the format.
 */
void log(Level level, const char* format, ...) {
  va_list args;
  va_start(args, format);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                  static_cast<SDL_LogPriority>(level), format, args);
  va_end(args);
}

/** Log an error message.
 * @param format The format of the message.
 * @param args The arguments to the format.
 */
template <typename... Args>
void error(const char* format, Args... args) {
  log(Level::Error, format, args...);
}

/** Log a warning message.
 * @param format The format of the message.
 * @param args The arguments to the format.
 */
template <typename... Args>
void warn(const char* format, Args... args) {
  log(Level::Warn, format, args...);
}

/** Log an info message.
 * @param format The format of the message.
 * @param args The arguments to the format.
 */
template <typename... Args>
void info(const char* format, Args... args) {
  log(Level::Info, format, args...);
}

}  // namespace Logger
