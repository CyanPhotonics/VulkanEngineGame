#ifndef TEXTURE_SAMPLERS_H
#define TEXTURE_SAMPLERS_H

#include <vector>
#include <sstream>

#include <vulkan/vulkan.h>

class TextureSampler {
private:
    std::vector<std::vector<VkImageView>> imageViews{};
    std::vector<std::vector<VkSampler>> samplers{};
    std::vector<std::vector<VkDescriptorImageInfo>> imageInfos{};
public:
    uint32_t bindingIndex = 0;
    size_t modelCount = 0;
    size_t arrayCount = 1;
    VkBool32 swapChainDependent;

    /*inline*/ VkImageView& accessImageView(uint32_t modelIndex, uint32_t arrayIndex);
    /*inline*/ VkImageView& accessImageView(uint32_t modelIndex);

    /*inline*/ VkSampler& accessSampler(uint32_t modelIndex, uint32_t arrayIndex);
    /*inline*/ VkSampler& accessSampler(uint32_t modelIndex);

    /*inline*/ VkDescriptorImageInfo& accessImageInfo(uint32_t modelIndex, uint32_t arrayIndex);
    /*inline*/ VkDescriptorImageInfo& accessImageInfo(uint32_t modelIndex);

    void resizeModelCount(size_t size) {
        modelCount = size;
        imageViews.resize(size);
        samplers.resize(size);
        imageInfos.resize(size);
    }

    void resizeArrayCount(size_t size) {
        modelCount = size;
        for (int i = 0; i < imageViews.size(); ++i) {
            imageViews[i].resize(size);
            samplers[i].resize(size);
            imageInfos[i].resize(size);
        }
    }
};

class StaticTextureSampler {
private:
    std::vector<VkImageView> imageViews{};
    std::vector<VkSampler> samplers{};
    std::vector<VkDescriptorImageInfo> imageInfos{};
public:
    uint32_t bindingIndex = 0;
    size_t modelCount = 0;
    size_t arrayCount = 1;
    VkBool32 swapChainDependent = VK_FALSE;

    /*inline*/ VkImageView& accessImageView(uint32_t arrayIndex);
    /*inline*/ VkImageView& accessImageView();

    /*inline*/ VkSampler& accessSampler(uint32_t arrayIndex);
    /*inline*/ VkSampler& accessSampler();

    /*inline*/ VkDescriptorImageInfo& accessImageInfo(uint32_t arrayIndex);
    /*inline*/ VkDescriptorImageInfo& accessImageInfo();

    void resizeArrayCount(size_t size){
        imageViews.resize(size);
        samplers.resize(size);
        imageInfos.resize(size);
    }
};

#endif //TEXTURE_SAMPLERS_H
