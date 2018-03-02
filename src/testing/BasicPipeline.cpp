#include "BasicPipeline.h"
#include "../resources/Model.h"

BasicPipeline::BasicPipeline(VulkanManager *vulkanManager, MemoryUtility* memoryUtility) : GraphicsPipelineBase(vulkanManager, memoryUtility) {}

void BasicPipeline::setShaderFiles(std::string &vertexFile, std::string &fragmentFile) {
    vertexFile = "vert.spv";
    fragmentFile = "frag.spv";
}

void BasicPipeline::setUniformVariables(size_t pointLightCount) {

    addUniformVariable(1, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), STATIC);
    addUniformVariable(1, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4));
    addUniformVariable(1, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PointLightUniformObject), STATIC);
    addTextureSamplerUniform(1, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT);

}

void BasicPipeline::setVertexInputBindings(std::vector<VkVertexInputBindingDescription> &bindings) {

    bindings.push_back(Vertex::getBindingDescription());

}

void BasicPipeline::setVertexInputAttributes(std::vector<VkVertexInputAttributeDescription> &attributes) {

    attributes = Vertex::getAttributeDescriptions();

}

void BasicPipeline::setBlendAttachments(std::vector<VkPipelineColorBlendAttachmentState> &blendAttachments) {

    blendAttachments.resize(1);

    blendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachments[0].blendEnable = VK_FALSE;


//    blendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//    blendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//    blendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD;
//    blendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//    blendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//    blendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD;

//    blendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//    blendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//    blendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD; // Optional
//    blendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//    blendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//    blendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD; // Optional

}

void BasicPipeline::setBlendInfo(VkPipelineColorBlendStateCreateInfo &blendInfo) {

    blendInfo.logicOpEnable = VK_FALSE;
//    blendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
//    blendInfo.blendConstants[0] = 0.0f; // Optional
//    blendInfo.blendConstants[1] = 0.0f; // Optional
//    blendInfo.blendConstants[2] = 0.0f; // Optional
//    blendInfo.blendConstants[3] = 0.0f; // Optional

}
