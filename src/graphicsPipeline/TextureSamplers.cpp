#include "TextureSamplers.h"

 VkImageView &TextureSampler::accessImageView(uint32_t modelIndex, uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= imageViews.size() || modelIndex < 0){
            output << "Index out of bounds error, accessing a per model image view vector.\n"
                   << "Accessed array size: "
                   << imageViews.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= imageViews[modelIndex].size() || arrayIndex < 0){
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

 VkImageView &TextureSampler::accessImageView(uint32_t modelIndex) {
    return accessImageView(modelIndex, 0);
}

 VkSampler &TextureSampler::accessSampler(uint32_t modelIndex, uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= samplers.size() || modelIndex < 0){
            output << "Index out of bounds error, accessing a per model sampler vector.\n"
                   << "Accessed array size: "
                   << samplers.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= samplers[modelIndex].size() || arrayIndex < 0){
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

 VkSampler &TextureSampler::accessSampler(uint32_t modelIndex) {
    return accessSampler(modelIndex, 0);
}

 VkDescriptorImageInfo &TextureSampler::accessImageInfo(uint32_t modelIndex, uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= imageInfos.size() || modelIndex < 0){
            output << "Index out of bounds error, accessing a per model image info vector.\n"
                   << "Accessed array size: "
                   << imageInfos.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= imageInfos[modelIndex].size() || arrayIndex < 0){
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

 VkDescriptorImageInfo &TextureSampler::accessImageInfo(uint32_t modelIndex) {
    return accessImageInfo(modelIndex, 0);
}

 VkImageView &StaticTextureSampler::accessImageView(uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        if (arrayIndex >= imageViews.size() || arrayIndex < 0){
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

 VkImageView &StaticTextureSampler::accessImageView() {
    return accessImageView(0);
}

 VkSampler &StaticTextureSampler::accessSampler(uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        if (arrayIndex >= samplers.size() || arrayIndex < 0){
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

 VkSampler &StaticTextureSampler::accessSampler() {
    return accessSampler(0);
}

 VkDescriptorImageInfo &StaticTextureSampler::accessImageInfo(uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        if (arrayIndex >= imageInfos.size() || arrayIndex < 0){
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

 VkDescriptorImageInfo &StaticTextureSampler::accessImageInfo() {
    return accessImageInfo(0);
}
