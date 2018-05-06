#ifndef TEXTURE_SAMPLERS_H
#define TEXTURE_SAMPLERS_H

#include <vector>
#include <sstream>

#include <vulkan/vulkan.hpp>

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

    inline VkImageView &accessImageView(const uint32_t &modelIndex, const uint32_t &arrayIndex) {
#ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= imageViews.size() || modelIndex < 0) {
            output << "Index out of bounds error, accessing a per model image view vector.\n"
                   << "Accessed array size: "
                   << imageViews.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= imageViews[modelIndex].size() || arrayIndex < 0) {
            output << "Index out of bounds error, accessing a per array image view buffer.\n"
                   << "Accessed array size: "
                   << imageViews[modelIndex].size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return imageViews[modelIndex][arrayIndex];
#else
        return imageViews[modelIndex][arrayIndex];
#endif
    }
    inline VkImageView &accessImageView(const uint32_t &modelIndex) { return accessImageView(modelIndex, 0); }

    inline VkSampler &accessSampler(const uint32_t &modelIndex, const uint32_t &arrayIndex) {
#ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= samplers.size() || modelIndex < 0) {
            output << "Index out of bounds error, accessing a per model sampler vector.\n"
                   << "Accessed array size: "
                   << samplers.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= samplers[modelIndex].size() || arrayIndex < 0) {
            output << "Index out of bounds error, accessing a per array sampler.\n"
                   << "Accessed array size: "
                   << samplers[modelIndex].size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return samplers[modelIndex][arrayIndex];
#else
        return samplers[modelIndex][arrayIndex];
#endif
    }
    inline VkSampler &accessSampler(const uint32_t &modelIndex) { return accessSampler(modelIndex, 0); }

    inline VkDescriptorImageInfo &accessImageInfo(const uint32_t &modelIndex, const uint32_t &arrayIndex) {
#ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= imageInfos.size() || modelIndex < 0) {
            output << "Index out of bounds error, accessing a per model image info vector.\n"
                   << "Accessed array size: "
                   << imageInfos.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= imageInfos[modelIndex].size() || arrayIndex < 0) {
            output << "Index out of bounds error, accessing a per array image info.\n"
                   << "Accessed array size: "
                   << imageInfos[modelIndex].size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return imageInfos[modelIndex][arrayIndex];
#else
        return imageInfos[modelIndex][arrayIndex];
#endif
    }
    inline VkDescriptorImageInfo &accessImageInfo(const uint32_t &modelIndex) { return accessImageInfo(modelIndex, 0); }

    void resizeModelCount(const size_t &size) {
        modelCount = size;
        imageViews.resize(size);
        samplers.resize(size);
        imageInfos.resize(size);
    }

    void resizeArrayCount(const size_t &size) {
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

    inline VkImageView &accessImageView(const uint32_t &arrayIndex) {
#ifndef NDEBUG // If Debug
        if (arrayIndex >= imageViews.size() || arrayIndex < 0) {
            std::stringstream output{};
            output << "Index out of bounds error, accessing a model static per array index, image view.\n"
                   << "Accessed array size: "
                   << imageViews.size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return imageViews[arrayIndex];
#else
        return imageViews[arrayIndex];
#endif
    }
    inline VkImageView &accessImageView() { return accessImageView(0); }

    inline VkSampler &accessSampler(const uint32_t &arrayIndex) {
#ifndef NDEBUG // If Debug
        if (arrayIndex >= samplers.size() || arrayIndex < 0) {
            std::stringstream output{};
            output << "Index out of bounds error, accessing a model static per array index, sampler.\n"
                   << "Accessed array size: "
                   << samplers.size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return samplers[arrayIndex];
#else
        return samplers[arrayIndex];
#endif
    }
    inline VkSampler &accessSampler() { return accessSampler(0); }

    inline VkDescriptorImageInfo &accessImageInfo(const uint32_t arrayIndex) {
#ifndef NDEBUG // If Debug
        if (arrayIndex >= imageInfos.size() || arrayIndex < 0) {
            std::stringstream output{};
            output << "Index out of bounds error, accessing a model static per array index, image info.\n"
                   << "Accessed array size: "
                   << imageInfos.size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return imageInfos[arrayIndex];
#else
        return imageInfos[arrayIndex];
#endif
    }
    inline VkDescriptorImageInfo &accessImageInfo() { return accessImageInfo(0); }

    void resizeArrayCount(size_t size) {
        imageViews.resize(size);
        samplers.resize(size);
        imageInfos.resize(size);
    }
};

#endif //TEXTURE_SAMPLERS_H
