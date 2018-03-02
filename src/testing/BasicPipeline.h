#ifndef BASIC_PIPELINE_H
#define BASIC_PIPELINE_H

#include "../rendering/GraphicsPipelineBase.h"

class BasicPipeline: public GraphicsPipelineBase {
private:
    void setShaderFiles(std::string &vertexFile, std::string &fragmentFile) override;
    void setVertexInputBindings(std::vector<VkVertexInputBindingDescription> &bindings) override;
    void setVertexInputAttributes(std::vector<VkVertexInputAttributeDescription> &attributes) override;
    void setBlendAttachments(std::vector<VkPipelineColorBlendAttachmentState> &blendAttachments) override;
    void setBlendInfo(VkPipelineColorBlendStateCreateInfo &blendInfo) override;
    void setUniformVariables(size_t pointLightCount) override;


public:
    explicit BasicPipeline(VulkanManager *vulkanManager, MemoryUtility* memoryUtility);

    struct PointLightUniformObject {
        glm::vec4 positions[MAX_POINT_LIGHTS];
        glm::vec4 attenuations[MAX_POINT_LIGHTS];
        glm::vec4 colours[MAX_POINT_LIGHTS];
        uint32_t count = 0;
    };

    PointLightUniformObject pointLightUO{};

};


#endif //BASIC_PIPELINE_H
