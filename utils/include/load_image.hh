/**
 * @brief Load a PPM image file into a buffer
 *
 * @param filename
 * @param width gets set to the image width
 * @param height gets set to the image height
 * @return unsigned char* if successful, nullptr otherwise
 */
unsigned char* loadPPM(const char* filename, int* width, int* height);