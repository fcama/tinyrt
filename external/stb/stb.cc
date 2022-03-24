#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Basic usage (see HDR discussion below for HDR usage):
// int width, height, comp;
// auto data = stbi_load("canvas.png", &width, &height, &comp, 0);
// if (!data)
// {
//     std::cout << "Cannot load canvas\n";
// }


// if(!stbi_write_png("output.jpg", width, height, comp, data, width * comp))
// {
//     std::cout << "Cannot write output\n";
// }

// stbi_image_free(data);