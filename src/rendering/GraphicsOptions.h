#ifndef GRAPHICS_OPTIONS_H
#define GRAPHICS_OPTIONS_H

#include <vulkan/vulkan.h>

enum class GaussianBlurLevels{
    DISABLED = 0,
    ENABLED_1 = 1,
    ENABLED_2 = 2,
    ENABLED_3 = 3
};

enum class GaussianBlurFactor{
    DISABLED = 0,
    ENABLED_1 = 1,
    ENABLED_1_2 = 2,
    ENABLED_1_4 = 4,
    ENABLED_1_8 = 8
};

class GraphicsOptions {
public:

    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

    GaussianBlurLevels gaussianBlurLevels = GaussianBlurLevels::DISABLED;
    GaussianBlurFactor gaussianBlurFactor = GaussianBlurFactor::DISABLED;

};

#endif //GRAPHICS_OPTIONS_H
