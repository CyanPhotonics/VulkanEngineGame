#include "BasicPipelines.h"

void BasicPipelines::createTexturedPipeline(PipelineTemplate &pipeline) {

    pipeline.subPassIndex = 0;

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

    // Depth

    pipeline.depthTestEnabled = VK_TRUE;
    pipeline.depthWriteEnabled = VK_TRUE;

    // Multisample

    pipeline.multiSample = VK_TRUE;


}

void BasicPipelines::createHorizontalGaussianBlurPipeline(PipelineTemplate &pipeline, int subpassIndex) {

    pipeline.subPassIndex = static_cast<uint32_t>(subpassIndex);

    // Shaders

    pipeline.vertexShaderFile = "blur/horizontal/vert.spv";
    pipeline.fragmentShaderFile = "blur/horizontal/frag.spv";

    //Uniforms

    pipeline.uniforms.emplace_back(0, 1, 0, VK_SHADER_STAGE_FRAGMENT_BIT, PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER_SWAP_CHAIN_DEPENDENT, PipelineTemplate::UniformTemplate::NON_STATIC);

    pipeline.uniforms.emplace_back(1, 1, sizeof(blurData), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::STATIC);

    // Blending

    pipeline.blendAttachments.resize(1);

    pipeline.blendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipeline.blendAttachments[0].blendEnable = VK_FALSE;

    pipeline.blendInfo.logicOpEnable = VK_FALSE;

    // Depth

    pipeline.depthTestEnabled = VK_FALSE;
    pipeline.depthWriteEnabled = VK_FALSE;

    // Multisample

    pipeline.multiSample = VK_FALSE;

}

void BasicPipelines::createVerticalGaussianBlurPipeline(PipelineTemplate &pipeline, int subpassIndex) {

    pipeline.subPassIndex = static_cast<uint32_t>(subpassIndex);

    // Shaders

    pipeline.vertexShaderFile = "blur/vertical/vert.spv";
    pipeline.fragmentShaderFile = "blur/vertical/frag.spv";

    //Uniforms

    pipeline.uniforms.emplace_back(0, 1, 0, VK_SHADER_STAGE_FRAGMENT_BIT, PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER_SWAP_CHAIN_DEPENDENT, PipelineTemplate::UniformTemplate::NON_STATIC);

    pipeline.uniforms.emplace_back(1, 1, sizeof(blurData), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, PipelineTemplate::UniformTemplate::VARIABLE, PipelineTemplate::UniformTemplate::STATIC);

    // Blending

    pipeline.blendAttachments.resize(1);

    pipeline.blendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipeline.blendAttachments[0].blendEnable = VK_FALSE;

    pipeline.blendInfo.logicOpEnable = VK_FALSE;

    // Depth

    pipeline.depthTestEnabled = VK_FALSE;
    pipeline.depthWriteEnabled = VK_FALSE;

    // Multisample

    pipeline.multiSample = VK_FALSE;

}

