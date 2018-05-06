#ifndef GAUSSIAN_BLUR_H
#define GAUSSIAN_BLUR_H

#include "PostProcessingStage.h"

class GaussianBlur {
private:
    float resolutionScale;

    PostProcessingStage horizontalStage;
    PostProcessingStage verticalStage;

public:
    GaussianBlur(uint32_t firstSubPassIndex, Texture sourceImage, Texture secondTexture, float resolutionScale, GraphicsPipelineManager* graphicsPipelineManager, VulkanManager* vulkanManager)
            : resolutionScale(resolutionScale),
              horizontalStage(sourceImage,resolutionScale,resolutionScale,firstSubPassIndex,graphicsPipelineManager,vulkanManager),
              verticalStage(secondTexture,resolutionScale,resolutionScale,firstSubPassIndex+1,graphicsPipelineManager,vulkanManager)
    {};

    float getResolutionScale() const {
        return resolutionScale;
    }

    const PostProcessingStage &getHorizontalStage() const {
        return horizontalStage;
    }

    const PostProcessingStage &getVerticalStage() const {
        return verticalStage;
    }

};

#endif //GAUSSIAN_BLUR_H
