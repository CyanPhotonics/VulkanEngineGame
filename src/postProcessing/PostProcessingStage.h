#ifndef POST_PROCESS_STAGE_H
#define POST_PROCESS_STAGE_H

#include "../rendering/ImageManager.h"
#include "../graphicsPipeline/GraphicsPipelineManager.h"

class PostProcessingStage {
private:

    uint32_t subpassIndex;

    Texture sourceTexture;
    std::array<uint32_t, 2> sourceSize{};
    //Texture outputTexture;
    std::array<uint32_t, 2> outputSize{};

public:
    PostProcessingStage(const Texture &sourceTexture, float sourceSizeScale,
                        float outputSizeScale, uint32_t subpassIndex,
                        GraphicsPipelineManager *graphicsPipelineManager, VulkanManager *vulkanManager)
            : subpassIndex(subpassIndex),
              sourceTexture(sourceTexture)/*, outputTexture(outputTexture)*/ {

        sourceSize = {static_cast<uint32_t>(vulkanManager->swapChainExtent.width * sourceSizeScale),
                      static_cast<uint32_t>(vulkanManager->swapChainExtent.height * sourceSizeScale)};
        outputSize = {static_cast<uint32_t>(vulkanManager->swapChainExtent.width * outputSizeScale),
                      static_cast<uint32_t>(vulkanManager->swapChainExtent.height * outputSizeScale)};

        graphicsPipelineManager->textureSamplers[subpassIndex][0].accessImageView(0) = sourceTexture.imageView;
        graphicsPipelineManager->textureSamplers[subpassIndex][0].accessSampler(0) = sourceTexture.sampler;
        graphicsPipelineManager->textureSamplers[subpassIndex][0].accessImageInfo(0).imageView = sourceTexture.imageView;
        graphicsPipelineManager->textureSamplers[subpassIndex][0].accessImageInfo(0).sampler = sourceTexture.sampler;
        graphicsPipelineManager->textureSamplers[subpassIndex][0].accessImageInfo(0).imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    }

    const Texture &getSourceTexture() const;

    const std::array<uint32_t, 2> &getSourceSize() const;

    const std::array<uint32_t, 2> &getOutputSize() const;

    uint32_t getSubpassIndex() const;

};

#endif //POST_PROCESS_STAGE_H
