#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H

#include <vector>
#include <stdexcept>
#include <unordered_map>

#include <stb_image.h>

#include "../vulkanSetup/VulkanManager.h"

#include "../rendering/CommandPoolManager.h"
#include "../resources/Texture.h"

#include "../utility/MemoryUtility.h"
#include "../scene/Scene.h"

class ImageManager {
private:
    VulkanManager* vulkanManager{};
    CommandPoolManager* commandPoolManager{};
    MemoryUtility* memoryUtility{};

    const char* textureFolder = "res/textures/";

    std::unordered_map<const char*, Texture> textures = {};


    void findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                     VkDeviceMemory &imageMemory, VkSampleCountFlagBits samples);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectFlags);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool hasStencilComponent(VkFormat format);
    void copyBufferToImage(VkBuffer buffer, VkImage &image, uint32_t width, uint32_t height);
    VkSampler createTextureSampler();
public:
    explicit ImageManager(VulkanManager* vulkanManager, CommandPoolManager* commandPoolManager, MemoryUtility* memoryUtility){
        this->vulkanManager = vulkanManager;
        this->commandPoolManager = commandPoolManager;
        this->memoryUtility = memoryUtility;
    }

    Texture MSAATexture{};

    void createDepthResources();
    Texture createAttachmentTexture(VkSampleCountFlagBits samples, VkImageUsageFlags usage = 0);
    Texture loadIntoLocal(const char *file);
    void loadIntoDevice(Texture &texture);

    void unloadFromLocal(Texture &texture);
    void unloadFromDevice(Texture &texture);

    void cleanUpDepthImage();
    void cleanUpTextures();

    void createMSAAResource();
    void createStorageImageResource(std::vector<Texture> &textures, float resScale);

    void cleanUpExtraImages(std::vector<Texture> &textures);

    Texture createAttachmentTexture(VkSampleCountFlagBits samples, VkImageUsageFlags usage, float resScale);
};


#endif //IMAGE_MANAGER_H
