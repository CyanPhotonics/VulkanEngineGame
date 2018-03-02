#include "GraphicsPipelineBase.h"

void GraphicsPipelineBase::setup() {

    //Custom shaders
    std::string vertexFile, fragmentFile;
    setShaderFiles(vertexFile, fragmentFile);
    loadShaderModules(vertexFile, fragmentFile);

    //Pipeline layout, aka Uniforms
    //setUniformVariables();
    createLayoutInfo();

    //Vertex Input
    std::vector<VkVertexInputBindingDescription> bindingsDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    setVertexInputBindings(bindingsDescriptions);
    setVertexInputAttributes(attributeDescriptions);
    createVertexInputInfo(bindingsDescriptions, attributeDescriptions);

    //Input assembly
    createInputAssemblyInfo();

    //Viewport/scissor
    createViewportInfo();

    //Rasterization
    createRasterizationInfo();

    //Multisampling
    createMultisampleInfo();

    //Blending
    setBlendAttachments(blendAttachments);
    createBlendInfo();
    setBlendInfo(blendInfo);

    //Dynamic state
    createDynamicStateInfo();

    //Depth/Stencil
    createDepthStencilInfo();

    //Finish
    createGraphicsPipeline();


}

void GraphicsPipelineBase::firstTimeOnlySetup(Scene *scene) {
    this->scene = scene;

    //Pipeline layout, aka Uniforms

    setUniformVariables(std::min(MAX_POINT_LIGHTS, scene->pointLights.size()));
    createLayoutInfo();

    textureSamplers[0].resize(scene->entities.size());
    for (int i = 0; i < scene->entities.size(); ++i) {
        textureSamplers[0].samplers[i] = scene->entities[i].texturedModel.texture.sampler;
        textureSamplers[0].imageViews[i] = scene->entities[i].texturedModel.texture.imageView;
    }

    //Stuff
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSet();

}

void GraphicsPipelineBase::loadShaderModules(std::string &vertexFile, std::string &fragmentFile) {

    auto vertexCode = readFile(vertexFile);
    auto fragmentCode = readFile(fragmentFile);

    shaderModules.resize(2);

    shaderModules[0] = createShaderModule(vertexCode);
    shaderModules[1] = createShaderModule(fragmentCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = shaderModules[0];
    vertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = shaderModules[1];
    fragmentShaderStageInfo.pName = "main";

    shaderStages = { vertexShaderStageInfo, fragmentShaderStageInfo };

}

std::vector<char> GraphicsPipelineBase::readFile(std::string fileName) {

    std::string filePath = shaderFolder;
    filePath += fileName;

    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        throw std::runtime_error("Failed to open file!\n");
    }

    auto fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;

}

VkShaderModule GraphicsPipelineBase::createShaderModule(std::vector<char> &data) {

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = data.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(data.data());

    VkShaderModule shaderModule{};
    if (vkCreateShaderModule(vulkanManager->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS){
        throw std::runtime_error("Failed to create shader module");
    }

    return shaderModule;

}

void GraphicsPipelineBase::addUniformVariable(uint32_t arrayCount, VkShaderStageFlags stageFlags, VkDeviceSize size,
                                              MemoryMode memoryMode) {

    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = static_cast<uint32_t>(bindings.size());
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = arrayCount;
    layoutBinding.stageFlags = stageFlags;

    int originalIndex = static_cast<int>(poolSizes.size());
    poolSizes.resize(poolSizes.size() + scene->entities.size());
    for (int i = 0; i < scene->entities.size(); ++i) {
        poolSizes[originalIndex + i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[originalIndex + i].descriptorCount = arrayCount;
    }

    if (memoryMode == STATIC){
        StaticUniformVariable staticUniformVariable{};
        staticUniformVariable.binding = lastBind++;
        staticUniformVariable.size = size;
        staticUniformVariable.arrayCount = arrayCount;
        totalStaticUniformArrayCount += arrayCount;
        staticUniformVariables.push_back(staticUniformVariable);
    } else {
        UniformVariable uniformVariable{};
        uniformVariable.binding = lastBind++;
        uniformVariable.size = size;
        uniformVariable.arrayCount = arrayCount;
        totalUniformArrayCount += arrayCount;
        uniformVariables.push_back(uniformVariable);
    }

    //uniformBufferSizes.push_back(size);

    bindings.push_back(layoutBinding);

}

void GraphicsPipelineBase::addTextureSamplerUniform(uint32_t arrayCount, VkSampler *immutableSamplers,
                                                    VkShaderStageFlags stageFlags, MemoryMode memoryMode) {

    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = static_cast<uint32_t>(bindings.size());
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBinding.pImmutableSamplers = immutableSamplers;
    layoutBinding.descriptorCount = arrayCount;
    layoutBinding.stageFlags = stageFlags;

    int originalIndex = static_cast<int>(poolSizes.size());
    poolSizes.resize(poolSizes.size() + scene->entities.size());
    for (int i = 0; i < scene->entities.size(); ++i) {
        poolSizes[originalIndex + i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[originalIndex + i].descriptorCount = arrayCount;
    }

    if (memoryMode == STATIC){
        StaticTextureSampler staticTextureSampler{};
        staticTextureSampler.binding = lastBind++;
        //staticTextureSampler.imageView =
        //staticTextureSampler.sampler =
        staticTextureSamplers.push_back(staticTextureSampler);
    } else {
        TextureSampler textureSampler{};
        textureSampler.binding = lastBind++;
        //textureSampler.imageViews.push_back();
        //textureSampler.samplers.push_back();
        textureSamplers.push_back(textureSampler);
    }

    bindings.push_back(layoutBinding);

}

void GraphicsPipelineBase::createLayoutInfo() {

    if (!pipelineLayouts.empty()){
        for (auto &pipelineLayout : pipelineLayouts){
            vkDestroyPipelineLayout(vulkanManager->device, pipelineLayout, nullptr);
        }
        pipelineLayouts.clear();
    }
    if (!descriptorSetLayouts.empty()){
        for (auto &descriptorLayout : descriptorSetLayouts){
            vkDestroyDescriptorSetLayout(vulkanManager->device, descriptorLayout, nullptr);
        }
        descriptorSetLayouts.clear();
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.flags = VkPipelineLayoutCreateFlags();
//    layoutInfo.pushConstantRangeCount = 0; // Optional
//    layoutInfo.pPushConstantRanges = 0; // Optional

    pipelineLayouts.resize(scene->entities.size());
    descriptorSetLayouts.resize(scene->entities.size());

    for (int i = 0; i < scene->entities.size(); ++i) {

        if (vkCreateDescriptorSetLayout(vulkanManager->device, &descriptorSetLayoutCreateInfo, nullptr,
                                        &descriptorSetLayouts[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout");
        }

        layoutInfo.pSetLayouts = &descriptorSetLayouts[i];

        //printf("Error 0: %s\n", GetLastError());

        if (vkCreatePipelineLayout (vulkanManager->device, &layoutInfo, nullptr, &pipelineLayouts[i]) != VK_SUCCESS) {
            throw std::runtime_error ("Failed to create pipeline layout");
        }
    }

}

void GraphicsPipelineBase::createUniformBuffer() {

    size_t modelCount = scene->entities.size();

    for (UniformVariable &uniformVariable : uniformVariables){
        uniformVariable.resizeModelCount(modelCount);
        uniformVariable.resizeArrayCount(uniformVariable.arrayCount);
        for (int i = 0; i < modelCount; ++i) {
            for (int j = 0; j < uniformVariable.arrayCount; ++j) {
                memoryUtility->createBuffer(uniformVariable.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            uniformVariable.buffers[i][j], uniformVariable.memorys[i][j]);
            }
        }
    }

    for (StaticUniformVariable &staticUniformVariable : staticUniformVariables){
        staticUniformVariable.resizeArrayCount(staticUniformVariable.arrayCount);
        for (int i = 0; i < staticUniformVariable.arrayCount; ++i) {
            memoryUtility->createBuffer(staticUniformVariable.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        staticUniformVariable.buffers[i], staticUniformVariable.memorys[i]);
        }
    }

}

void GraphicsPipelineBase::createDescriptorPool() {

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = static_cast<uint32_t>(scene->entities.size());

    if (vkCreateDescriptorPool(vulkanManager->device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Descriptor pool");
    }

}

void GraphicsPipelineBase::createDescriptorSet(){

    std::vector<std::vector<VkDescriptorBufferInfo>> staticBufferInfos{};
    staticBufferInfos.resize(staticUniformVariables.size());

    for (int i = 0; i < staticUniformVariables.size(); ++i) {
        staticBufferInfos[i].resize(staticUniformVariables[i].arrayCount);
        for (int j = 0; j < staticBufferInfos[i].size(); ++j) {
            staticBufferInfos[i][j].buffer = staticUniformVariables[i].buffers[j];
            staticBufferInfos[i][j].offset = 0;
            staticBufferInfos[i][j].range = staticUniformVariables[i].size;
        }
    }

    std::vector<VkDescriptorImageInfo> staticImageInfos{};
    staticImageInfos.resize(staticTextureSamplers.size());

    for (int i = 0; i < staticImageInfos.size(); ++i) {
        staticImageInfos[i].imageView = staticTextureSamplers[i].imageView;
        staticImageInfos[i].sampler = staticTextureSamplers[i].sampler;
        staticImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();

    descriptorSets.resize(descriptorSetLayouts.size());
    if (vkAllocateDescriptorSets (vulkanManager->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error ("Failed to allocate descriptor set!\n");
    }

    for (int i = 0; i < descriptorSetLayouts.size(); ++i) {

        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos{};
        bufferInfos.resize(totalUniformArrayCount);

        for (int j = 0; j < uniformVariables.size(); ++j) {
            bufferInfos[j].resize(uniformVariables[j].arrayCount);
            for (int k = 0; k < bufferInfos[j].size(); ++k) {
                bufferInfos[j][k].buffer = uniformVariables[j].buffers[i][k];
                bufferInfos[j][k].offset = 0;
                bufferInfos[j][k].range = uniformVariables[j].size;
            }
        }

        std::vector<VkDescriptorImageInfo> imageInfos{};
        imageInfos.resize(textureSamplers.size());

        for (int j = 0; j < imageInfos.size(); ++j) {
            imageInfos[j].imageView = textureSamplers[j].imageViews[i];
            imageInfos[j].sampler = textureSamplers[j].samplers[i];
            imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites = {};

        for (int j = 0 ; j < staticBufferInfos.size() ; ++j){
            for (int k = 0; k < staticBufferInfos[j].size(); ++k) {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[i];
                descriptorWrite.dstBinding = staticUniformVariables[j].binding;
                descriptorWrite.dstArrayElement = static_cast<uint32_t>(k);
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &staticBufferInfos[j][k];
                descriptorWrites.push_back(descriptorWrite);
            }
        }

        for (int j = 0; j < bufferInfos.size(); ++j) {
            for (int k = 0; k < bufferInfos[j].size(); ++k) {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[i];
                descriptorWrite.dstBinding = uniformVariables[j].binding;
                descriptorWrite.dstArrayElement = static_cast<uint32_t>(k);
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfos[j][k];
                descriptorWrites.push_back(descriptorWrite);
            }
        }

        for (int j = 0; j < staticImageInfos.size(); ++j) {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = staticTextureSamplers[j].binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &staticImageInfos[j];
            descriptorWrites.push_back(descriptorWrite);
        }

        for (int j = 0; j < imageInfos.size(); ++j) {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = textureSamplers[j].binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfos[j];
            descriptorWrites.push_back(descriptorWrite);
        }

        vkUpdateDescriptorSets (vulkanManager->device, static_cast<uint32_t>(descriptorWrites.size ()), descriptorWrites.data (), 0, nullptr);

    }

}

void GraphicsPipelineBase::createVertexInputInfo(std::vector<VkVertexInputBindingDescription> &bindings, std::vector<VkVertexInputAttributeDescription> &attributes) {

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
    vertexInputInfo.pVertexBindingDescriptions = bindings.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

}

void GraphicsPipelineBase::createInputAssemblyInfo() {

    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

}

void GraphicsPipelineBase::createViewportInfo() {

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkanManager->swapChainExtent.width;
    viewport.height = (float)vulkanManager->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = { 0,0 };
    scissor.extent = vulkanManager->swapChainExtent;

    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

}

void GraphicsPipelineBase::createRasterizationInfo() {

    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.depthClampEnable = VK_FALSE;
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationInfo.depthBiasEnable = VK_FALSE;
    rasterizationInfo.depthBiasConstantFactor = 0.0f;
    rasterizationInfo.depthBiasClamp = 0.0f;
    rasterizationInfo.depthBiasSlopeFactor = 0.0f;

}

void GraphicsPipelineBase::createMultisampleInfo() {

    multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.sampleShadingEnable = ((vulkanManager->samples == VK_SAMPLE_COUNT_1_BIT) ? VK_FALSE : VK_TRUE);
    multisampleInfo.rasterizationSamples = vulkanManager->samples;
    multisampleInfo.minSampleShading = 1.0f; // Optional
    multisampleInfo.pSampleMask = nullptr; // Optional
    multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

}

void GraphicsPipelineBase::createBlendInfo() {
    blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendInfo.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
    blendInfo.pAttachments = blendAttachments.data();
}

void GraphicsPipelineBase::createDynamicStateInfo() {

//    VkDynamicState dynamicStates[] = {
//
//    };
//
//    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicStateInfo.dynamicStateCount = 0;
//    dynamicStateInfo.pDynamicStates = dynamicStates;


}

void GraphicsPipelineBase::createDepthStencilInfo() {

    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;
    depthStencilInfo.depthWriteEnable = VK_TRUE;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
//    depthStencilInfo.minDepthBounds = 0.0f; // Optional
//    depthStencilInfo.maxDepthBounds = 1.0f; // Optional
    depthStencilInfo.stencilTestEnable = VK_FALSE;
//    depthStencilInfo.front = {}; // Optional
//    depthStencilInfo.back = {}; // Optional

}

void GraphicsPipelineBase::createGraphicsPipeline() {

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    createInfo.pStages = shaderStages.data();
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &inputAssemblyInfo;
    createInfo.pViewportState = &viewportInfo;
    createInfo.pRasterizationState = &rasterizationInfo;
    createInfo.pMultisampleState = &multisampleInfo;
    createInfo.pDepthStencilState = &depthStencilInfo;
    createInfo.pColorBlendState = &blendInfo;
    createInfo.pDynamicState = nullptr;
    createInfo.layout = pipelineLayouts[0];
    createInfo.renderPass = vulkanManager->renderPass;
    createInfo.subpass = 0;

//    createInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//    createInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines (vulkanManager->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error ("Failed to create Graphics pipeline");
    }

    for (VkShaderModule shaderModule : shaderModules){
        vkDestroyShaderModule (vulkanManager->device, shaderModule, nullptr);
    }

}

void GraphicsPipelineBase::cleanUp() {

    for (auto &descriptorSetLayout : descriptorSetLayouts){
        vkDestroyDescriptorSetLayout(vulkanManager->device, descriptorSetLayout, nullptr);
    }
    descriptorSetLayouts.clear();

    for (auto &pipelineLayout : pipelineLayouts) {
        vkDestroyPipelineLayout(vulkanManager->device, pipelineLayout, nullptr);
    }
    pipelineLayouts.clear();

    vkDestroyPipeline(vulkanManager->device,pipeline, nullptr);

    pipeline = VK_NULL_HANDLE;

}

void GraphicsPipelineBase::cleanUpFinal() {

    //cleanUp();
    for (auto &descriptorSetLayout : descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(vulkanManager->device, descriptorSetLayout, nullptr);
    }
    descriptorSetLayouts.clear();

    for (UniformVariable &uniformVariable : uniformVariables){
        for (int i = 0 ; i < scene->entities.size() ; i++){
            for (int j = 0; j < uniformVariable.buffers[i].size(); ++j) {
                vkDestroyBuffer(vulkanManager->device, uniformVariable.buffers[i][j], nullptr);
                vkFreeMemory(vulkanManager->device, uniformVariable.memorys[i][j], nullptr);
            }
        }
        uniformVariable.memorys.clear();
        uniformVariable.buffers.clear();
    }
    uniformVariables.clear();

    for (StaticUniformVariable &staticUniformVariable : staticUniformVariables){
        for (int i = 0; i < staticUniformVariable.arrayCount; ++i) {
            vkDestroyBuffer(vulkanManager->device, staticUniformVariable.buffers[i], nullptr);
            vkFreeMemory(vulkanManager->device, staticUniformVariable.memorys[i], nullptr);
        }
        staticUniformVariable.memorys.clear();
        staticUniformVariable.buffers.clear();
    }
    staticUniformVariables.clear();

    for (TextureSampler &textureSampler : textureSamplers){
        textureSampler.imageViews.clear();
        textureSampler.samplers.clear();
    }
    uniformVariables.clear();

    for (StaticTextureSampler &staticTextureSampler : staticTextureSamplers){
        staticTextureSampler.imageView = VK_NULL_HANDLE;
        staticTextureSampler.sampler = VK_NULL_HANDLE;
    }
    staticUniformVariables.clear();

    vkDestroyDescriptorPool(vulkanManager->device, descriptorPool, nullptr);
    descriptorPool = VK_NULL_HANDLE;

}
