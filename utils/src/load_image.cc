#include "load_image.hh"

#include <fstream>
#include <iostream>
#include <string>

unsigned char* loadPPM(const char* filename, int* width, int* height) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Could not open file " << filename << std::endl;
    return nullptr;
  }

  std::string magicNumber;
  file >> magicNumber;
  if (magicNumber != "P6") {
    std::cerr << "Invalid magic number " << magicNumber << std::endl;
    return nullptr;
  }

  file >> *width >> *height;
  int maxColorValue;
  file >> maxColorValue;
  if (maxColorValue != 255) {
    std::cerr << "Invalid max color value " << maxColorValue << std::endl;
    return nullptr;
  }

  unsigned char* buffer = new unsigned char[3 * *width * *height];
  file.read(reinterpret_cast<char*>(buffer), 3 * *width * *height);
  file.close();

  return buffer;
}