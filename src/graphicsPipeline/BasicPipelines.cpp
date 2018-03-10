#include "BasicPipelines.h"

void BasicPipelines::createTexturedPipeline(PipelineTemplate &pipeline) {

    pipeline.subpass = 0;

    // Shaders

    pipeline.vertexShaderFile = "textured/vert.spv";
    pipeline.fragmentShaderFile = "textured/frag.spv";

    // Uniforms

    // viewProjectionMatrix
    pipeline.uniforms.emplace_back(0, 1, sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::STATIC);
    // modelMatrix
    pipeline.uniforms.emplace_back(1, 1, sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::NON_STATIC);
    // point light objects
    pipeline.uniforms.emplace_back(2, 1, sizeof(PointLightUniformObject), VK_SHADER_STAGE_FRAGMENT_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::STATIC);
    // Texture sampler
    pipeline.uniforms.emplace_back(3, 1, 0, VK_SHADER_STAGE_FRAGMENT_BIT, PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER, PipelineTemplate::UniformTemplate::NON_STATIC);

    // Vertex Input

    pipeline.vertexInputBindings = {Vertex::getBindingDescription()};
    pipeline.vertexInputAttributes = Vertex::getAttributeDescriptions();

    // Blending

    pipeline.blendAttachments.resize(1);

    pipeline.blendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipeline.blendAttachments[0].blendEnable = VK_FALSE;

    pipeline.blendInfo.logicOpEnable = VK_FALSE;


}

void BasicPipelines::createUntexturedUnlitPipeline(PipelineTemplate &pipeline) {

    pipeline.subpass = 1;

    // Shaders

    pipeline.vertexShaderFile = "blank/vert.spv";
    pipeline.fragmentShaderFile = "blank/frag.spv";

    // Uniforms

    // viewProjectionMatrix
    pipeline.uniforms.emplace_back(0, 1, sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::STATIC);
    // modelMatrix
    pipeline.uniforms.emplace_back(1, 1, sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::NON_STATIC);

    // Vertex Input

    pipeline.vertexInputBindings = {Vertex::getBindingDescription()};
    pipeline.vertexInputAttributes = Vertex::getAttributeDescriptions();

    // Blending

    pipeline.blendAttachments.resize(1);

    pipeline.blendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipeline.blendAttachments[0].blendEnable = VK_FALSE;

    pipeline.blendInfo.logicOpEnable = VK_FALSE;

}

