#define STB_IMAGE_IMPLEMENTATION

#include "ImageManager.h"
#include "../scene/Scene.h"

Texture ImageManager::loadIntoLocal(const char *file){

    if (textures.count(file) != 0){
        return textures.find(file)->second;
    }

    Texture texture{};

    std::string filePath = textureFolder;
    filePath += file;

    //printf("File: %s\n", filePath.c_str());

    texture.pixels = stbi_load(filePath.c_str(), &texture.texWidth, &texture.texHeight, &texture.texChannels, STBI_rgb_alpha);

    texture.file = file;

    if (!texture.pixels){
        throw std::runtime_error("Failed to load texture file: " + filePath);
    }

    texture.isLoadedIntoLocal = true;

    textures[file] = texture;

    return texture;

}

void ImageManager::loadIntoDevice(Texture &texture) {

    if (!texture.isLoadedIntoLocal || texture.isLoadedIntoDevice){
        return;
    }

    VkDeviceSize imageSize = texture.texWidth * texture.texHeight * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    memoryUtility->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanManager->device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, texture.pixels, static_cast<size_t >(imageSize));
    vkUnmapMemory(vulkanManager->device, stagingBufferMemory);

    //unloadFromLocal(texture);

    createImage(static_cast<uint32_t>(texture.texWidth), static_cast<uint32_t>(texture.texHeight), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                texture.image, texture.deviceMemory, VK_SAMPLE_COUNT_1_BIT);

    transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    copyBufferToImage(stagingBuffer, texture.image, static_cast<uint32_t>(texture.texWidth), static_cast<uint32_t>(texture.texHeight));

    transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(vulkanManager->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanManager->device, stagingBufferMemory, nullptr);

    texture.imageView = createImageView(texture.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
    texture.sampler = createTextureSampler();

    texture.isLoadedIntoDevice = true;

    textures[texture.file] = texture;

}

int internalTextureIndex = 0;

Texture ImageManager::createAttachmentTexture(VkSampleCountFlagBits samples, VkImageUsageFlags usage) {

    return createAttachmentTexture(samples, usage, 1.0f);

}

Texture ImageManager::createAttachmentTexture(VkSampleCountFlagBits samples, VkImageUsageFlags usage, float resScale) {

    Texture texture{};
    texture.file = ".internal." + internalTextureIndex++;

    createImage(static_cast<uint32_t>(vulkanManager->swapChainExtent.width * resScale),
                static_cast<uint32_t>(vulkanManager->swapChainExtent.height * resScale),
                vulkanManager->swapChainImageFormat,
                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (usage), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture.image,
                texture.deviceMemory, samples);

    texture.imageView = createImageView(texture.image, vulkanManager->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    texture.sampler = createTextureSampler();

    transitionImageLayout(texture.image, vulkanManager->swapChainImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    texture.isLoadedIntoDevice = true;
    textures[texture.file].isLoadedIntoDevice = true;

    return texture;

}

void ImageManager::createDepthResources() {

    findDepthFormat();

    createImage(vulkanManager->swapChainExtent.width, vulkanManager->swapChainExtent.height,
                vulkanManager->depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanManager->depthImage, vulkanManager->depthMemory, vulkanManager->samples);

    vulkanManager->depthImageView = createImageView(vulkanManager->depthImage, vulkanManager->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    transitionImageLayout (vulkanManager->depthImage, vulkanManager->depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

void ImageManager::createMSAAResource() {

    MSAATexture = createAttachmentTexture(vulkanManager->samples);

}

void ImageManager::createStorageImageResource(std::vector<Texture> &textures, float resScale) {

    Texture texture = createAttachmentTexture(VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT, resScale);

    textures.push_back(texture);

}

void ImageManager::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                               VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                               VkDeviceMemory &imageMemory, VkSampleCountFlagBits samples) {

    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = tiling;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples = samples;

    if(vkCreateImage(vulkanManager->device, &createInfo, nullptr, &image) != VK_SUCCESS){
        throw std::runtime_error("Failed to create image!\n");
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetImageMemoryRequirements(vulkanManager->device, image, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryUtility->findMemoryType (memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanManager->device, &allocateInfo, nullptr, &imageMemory) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate device memory");
    }

    vkBindImageMemory(vulkanManager->device, image, imageMemory, 0);

}



VkImageView ImageManager::createImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectFlags) {

    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.format = format;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(vulkanManager->device, &createInfo, nullptr, &imageView) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Image View!\n");
    }

    return imageView;

}

void ImageManager::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                                         VkImageLayout newLayout) {

    VkCommandBuffer commandBuffer = commandPoolManager->beginSingleTimeCommand(vulkanManager->graphicsCommandPool);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent (format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

    }
    else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&  newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL){
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::runtime_error ("Unsupported layout transition!\n");
    }

    vkCmdPipelineBarrier (
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    commandPoolManager->endSingleTimeCommand (commandBuffer, vulkanManager->graphicsCommandPool);

}

void ImageManager::findDepthFormat() {
    vulkanManager->depthFormat = findSupportedFormat ({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                                        VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
                                                      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat ImageManager::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                           VkFormatFeatureFlags features) {

    for (VkFormat format : candidates) {

        VkFormatProperties props{};
        vkGetPhysicalDeviceFormatProperties (vulkanManager->physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }

    }

    throw std::runtime_error ("Failed to find supported format!\n");

}

bool ImageManager::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void ImageManager::copyBufferToImage(VkBuffer buffer, VkImage &image, uint32_t width, uint32_t height) {

    VkCommandBuffer commandBuffer = commandPoolManager->beginSingleTimeCommand(vulkanManager->transferCommandPool);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength= 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0,0,0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandPoolManager->endSingleTimeCommand(commandBuffer, vulkanManager->transferCommandPool);


}

VkSampler ImageManager::createTextureSampler() {

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; //TODO maybe do something with
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.0f;

    VkSampler sampler{};

    if (vkCreateSampler(vulkanManager->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS){
        throw std::runtime_error("Failed to create sampler!\n");
    }

    return sampler;

}

void ImageManager::unloadFromLocal(Texture &texture) {

    if (!texture.isLoadedIntoLocal || !textures[texture.file].isLoadedIntoLocal){
        return;
    }

    if (texture.pixels != nullptr && textures[texture.file].pixels != nullptr) {
        stbi_image_free(texture.pixels);
    }

    texture.pixels = nullptr;
    texture.isLoadedIntoLocal = false;
    textures[texture.file].pixels = nullptr;
    textures[texture.file].isLoadedIntoLocal = false;

}

void ImageManager::unloadFromDevice(Texture &texture) {

    if (!texture.isLoadedIntoDevice || !textures[texture.file].isLoadedIntoDevice){
        return;
    }

    vkDestroySampler(vulkanManager->device, texture.sampler, nullptr);
    vkDestroyImageView(vulkanManager->device, texture.imageView, nullptr);
    vkDestroyImage(vulkanManager->device, texture.image, nullptr);
    vkFreeMemory(vulkanManager->device, texture.deviceMemory, nullptr);

    texture.sampler = VK_NULL_HANDLE;
    texture.imageView = VK_NULL_HANDLE;
    texture.image = VK_NULL_HANDLE;
    texture.deviceMemory = VK_NULL_HANDLE;
    texture.isLoadedIntoDevice = false;
    textures[texture.file].sampler = VK_NULL_HANDLE;
    textures[texture.file].imageView = VK_NULL_HANDLE;
    textures[texture.file].image = VK_NULL_HANDLE;
    textures[texture.file].deviceMemory = VK_NULL_HANDLE;
    textures[texture.file].isLoadedIntoDevice = false;

    //textures[texture.file] = texture;

}

void ImageManager::cleanUpDepthImage() {

    vkDestroyImageView(vulkanManager->device, vulkanManager->depthImageView, nullptr);
    vkDestroyImage(vulkanManager->device, vulkanManager->depthImage, nullptr);
    vkFreeMemory(vulkanManager->device, vulkanManager->depthMemory, nullptr);

    vulkanManager->depthImageView = VK_NULL_HANDLE;
    vulkanManager->depthImage = VK_NULL_HANDLE;
    vulkanManager->depthMemory = VK_NULL_HANDLE;

}

void ImageManager::cleanUpExtraImages(std::vector<Texture> &textures) {

    unloadFromDevice(MSAATexture);

    for (Texture &texture : textures){
        unloadFromDevice(texture);
    }
    textures.clear();

}

void ImageManager::cleanUpTextures() {

    for (auto textureIter : textures){

        unloadFromDevice(textureIter.second);
        unloadFromLocal(textureIter.second);

    }

}
