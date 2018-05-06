#ifndef BASIC_PIPELINES_H
#define BASIC_PIPELINES_H

#include "../graphicsPipeline/GraphicsPipelineManager.h"

class BasicPipelines {
private:
    void createTexturedPipeline(PipelineTemplate &pipeline);

    void createHorizontalGaussianBlurPipeline(PipelineTemplate &pipeline, int subpassIndex);

    void createVerticalGaussianBlurPipeline(PipelineTemplate &pipeline, int subpassIndex);

public:
    std::vector<PipelineTemplate> pipelines;

    void createPipelines(VulkanManager* vulkanManager){

        pipelines.resize(1 + 2 * (size_t) vulkanManager->graphicsOptions.gaussianBlurLevels);

        createTexturedPipeline(pipelines[0]);

        for (int i = 0; i < (int) vulkanManager->graphicsOptions.gaussianBlurLevels; ++i) {

            createHorizontalGaussianBlurPipeline(pipelines[2*i + 1], 2*i + 1);
            createVerticalGaussianBlurPipeline(pipelines[2*i + 2], 2*i + 2);

        }

    }

    struct PointLightUniformObject {
        glm::vec4 positions[MAX_POINT_LIGHTS];
        glm::vec4 attenuations[MAX_POINT_LIGHTS];
        glm::vec4 colours[MAX_POINT_LIGHTS];
        uint32_t count = 0;
    } pointLightUO;

    struct BlurData {
        uint32_t imageWidth;
        uint32_t imageHeight;
        float_t resScale;
    private:
        float_t temp = 0;
    } blurData;

};

#endif //BASIC_PIPELINES_H
