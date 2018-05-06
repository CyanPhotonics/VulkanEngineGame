#ifndef TEXTURE_H
#define TEXTURE_H

#include <vulkan/vulkan.hpp>

//#include <stb_image.h>

class Texture {
public:
    bool isLoadedIntoDevice = false;
    bool isLoadedIntoLocal = false;

    const char* file = "";
    stbi_uc* pixels{};
    int texWidth = 0, texHeight = 0, texChannels = 0;

    VkImage image{};
    VkDeviceMemory deviceMemory{};
    VkImageView imageView{};
    VkSampler sampler{};
};

#endif //TEXTURE_H
