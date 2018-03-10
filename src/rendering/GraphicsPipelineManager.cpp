#include "GraphicsPipelineManager.h"

void GraphicsPipelineManager::setup() {

    pipelineLayouts.resize(pipelineTemplates.size());
    pipelines.resize(pipelineTemplates.size());

    for (int i = 0; i < pipelineTemplates.size(); ++i) {

        //Shaders
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{};
        loadShaderModules(shaderStageCreateInfos, pipelineTemplates[i]);

        //Pipeline layout
        createLayoutInfo(pipelineLayouts[i]);

        // Vertex input
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        createVertexInputInfo(vertexInputStateCreateInfo, pipelineTemplates[i].vertexInputBindings,
                              pipelineTemplates[i].vertexInputAttributes);

        // Assembly input
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
        createInputAssemblyInfo(inputAssemblyStateCreateInfo);

        // Viewport/scissor
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        createViewportInfo(viewportStateCreateInfo);

        //Rasterization
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
        createRasterizationInfo(rasterizationStateCreateInfo);

        //Multisampling
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        createMultisampleInfo(multisampleStateCreateInfo);

        //Blending
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
        createBlendInfo(colorBlendStateCreateInfo, pipelineTemplates[i]);

        //Dynamic state //TODO work on
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        createDynamicStateInfo(dynamicStateCreateInfo);

        //Depth/Stencil
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
        createDepthStencilInfo(depthStencilStateCreateInfo);

        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
        createInfo.pStages = shaderStageCreateInfos.data();
        createInfo.pVertexInputState = &vertexInputStateCreateInfo;
        createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        createInfo.pViewportState = &viewportStateCreateInfo;
        createInfo.pRasterizationState = &rasterizationStateCreateInfo;
        createInfo.pMultisampleState = &multisampleStateCreateInfo;
        createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        createInfo.pColorBlendState = &colorBlendStateCreateInfo;
        createInfo.pDynamicState = nullptr;
        createInfo.layout = pipelineLayouts[i][0];
        createInfo.renderPass = vulkanManager->renderPass;
        createInfo.subpass = pipelineTemplates[i].subpass;

        if (vkCreateGraphicsPipelines (vulkanManager->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipelines[i]) != VK_SUCCESS) {
            throw std::runtime_error ("Failed to create Graphics pipeline");
        }

        for (VkShaderModule shaderModule : shaderModules){
            vkDestroyShaderModule (vulkanManager->device, shaderModule, nullptr);
        }
        shaderModules.clear();

    }

}

void GraphicsPipelineManager::firstTimeOnlySetup(std::vector<PipelineTemplate> &pipelineTemplates, Scene *scene) {
    this->pipelineTemplates = pipelineTemplates;
    this->scene = scene;

    //PipelineTemplate layout, aka Uniforms

    ///setUniformVariables(std::min(MAX_POINT_LIGHTS, scene->pointLights.size()));

    pipelineLayouts.resize(pipelineTemplates.size());

    uniformVariables.resize(pipelineTemplates.size());
    staticUniformVariables.resize(pipelineTemplates.size());
    textureSamplers.resize(pipelineTemplates.size());
    staticTextureSamplers.resize(pipelineTemplates.size());

    for (int i = 0; i < pipelineTemplates.size(); ++i) {

        createUniforms(static_cast<uint32_t>(i), pipelineTemplates[i].uniforms);
        createLayoutInfo(pipelineLayouts[i]);

    }

    //TODO fix
    textureSamplers[0][0].resize(scene->entities.size());
    for (int i = 0; i < scene->entities.size(); ++i) {
        textureSamplers[0][0].samplers[i] = scene->entities[i].texturedModel.texture.sampler;
        textureSamplers[0][0].imageViews[i] = scene->entities[i].texturedModel.texture.imageView;
    }
//    textureSamplers[1][0].resize(scene->entities.size());
//    for (int i = 0; i < scene->entities.size(); ++i) {
//        textureSamplers[1][0].samplers[i] = scene->entities[i].texturedModel.texture.sampler;
//        textureSamplers[1][0].imageViews[i] = scene->entities[i].texturedModel.texture.imageView;
//    }

    descriptorSets.resize(pipelineTemplates.size());

    for (int i = 0; i < pipelineTemplates.size(); ++i) {
        //Stuff
        createUniformBuffer(static_cast<uint32_t>(i));
    }
    createDescriptorPool();
    for (int i = 0; i < pipelineTemplates.size(); ++i) {
        createDescriptorSet(static_cast<uint32_t>(i));

    }

}

void GraphicsPipelineManager::loadShaderModules(std::vector<VkPipelineShaderStageCreateInfo> &shaderStageCreateInfos,
                                                PipelineTemplate &pipelineTemplate) {

    auto vertexCode = readFile(pipelineTemplate.vertexShaderFile);
    auto fragmentCode = readFile(pipelineTemplate.fragmentShaderFile);

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

    shaderStageCreateInfos.push_back(vertexShaderStageInfo);
    shaderStageCreateInfos.push_back(fragmentShaderStageInfo);

}

std::vector<char> GraphicsPipelineManager::readFile(std::string fileName) {

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

VkShaderModule GraphicsPipelineManager::createShaderModule(std::vector<char> &data) {

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

void GraphicsPipelineManager::createUniforms(uint32_t pipelineIndex,
                                             std::vector<PipelineTemplate::UniformTemplate> uniforms) {


    uniformVariables[pipelineIndex].clear();
    staticUniformVariables[pipelineIndex].clear();
    textureSamplers[pipelineIndex].clear();
    staticTextureSamplers[pipelineIndex].clear();

    for (PipelineTemplate::UniformTemplate uniform : uniforms){

        if (uniform.uniformType == PipelineTemplate::UniformTemplate::VARIABLE){
            addUniformVariable(pipelineIndex, uniform.index, uniform.arrayCount, uniform.stageFlags, uniform.size, uniform.memoryMode);
        } else if (uniform.uniformType == PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER){
            addTextureSamplerUniform(pipelineIndex, uniform.index, uniform.arrayCount, nullptr, uniform.stageFlags, uniform.memoryMode);
        }

    }

}

void GraphicsPipelineManager::addUniformVariable(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                                 VkShaderStageFlags stageFlags, VkDeviceSize size,
                                                 PipelineTemplate::UniformTemplate::MemoryMode memoryMode) {

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

    if (memoryMode == PipelineTemplate::UniformTemplate::MemoryMode::STATIC){
        StaticUniformVariable staticUniformVariable{};
        staticUniformVariable.binding = bindingIndex;
        staticUniformVariable.size = size;
        staticUniformVariable.arrayCount = arrayCount;
        totalStaticUniformArrayCount += arrayCount;
        staticUniformVariables[pipelineIndex].push_back(staticUniformVariable);
    } else {
        UniformVariable uniformVariable{};
        uniformVariable.binding = bindingIndex;
        uniformVariable.size = size;
        uniformVariable.arrayCount = arrayCount;
        totalUniformArrayCount += arrayCount;
        uniformVariables[pipelineIndex].push_back(uniformVariable);
    }

    //uniformBufferSizes.push_back(size);

    bindings.push_back(layoutBinding);

}

void GraphicsPipelineManager::addTextureSamplerUniform(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                                       VkSampler *immutableSamplers, VkShaderStageFlags stageFlags,
                                                       PipelineTemplate::UniformTemplate::MemoryMode memoryMode) {

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

    if (memoryMode == PipelineTemplate::UniformTemplate::STATIC){
        StaticTextureSampler staticTextureSampler{};
        staticTextureSampler.binding = bindingIndex;
        //staticTextureSampler.imageView =
        //staticTextureSampler.sampler =
        staticTextureSamplers[pipelineIndex].push_back(staticTextureSampler);
    } else {
        TextureSampler textureSampler{};
        textureSampler.binding = bindingIndex;
        //textureSampler.imageViews.push_back();
        //textureSampler.samplers.push_back();
        textureSamplers[pipelineIndex].push_back(textureSampler);
    }

    bindings.push_back(layoutBinding);

}

void GraphicsPipelineManager::createLayoutInfo(std::vector<VkPipelineLayout> &pipelineLayouts) {

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
    auto offset = static_cast<int>(descriptorSetLayouts.size());
    descriptorSetLayouts.resize(descriptorSetLayouts.size() + scene->entities.size());

    for (int i = 0; i < scene->entities.size(); ++i) {

        if (vkCreateDescriptorSetLayout(vulkanManager->device, &descriptorSetLayoutCreateInfo, nullptr,
                                        &descriptorSetLayouts[i + offset]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout");
        }

        layoutInfo.pSetLayouts = &descriptorSetLayouts[i + offset];

        //printf("Error 0: %s\n", GetLastError());

        if (vkCreatePipelineLayout (vulkanManager->device, &layoutInfo, nullptr, &pipelineLayouts[i]) != VK_SUCCESS) {
            throw std::runtime_error ("Failed to create pipeline layout");
        }
    }

}

void GraphicsPipelineManager::createUniformBuffer(uint32_t pipelineIndex) {

    size_t modelCount = scene->entities.size();

    for (UniformVariable &uniformVariable : uniformVariables[pipelineIndex]){
        uniformVariable.resizeModelCount(modelCount);
        uniformVariable.resizeArrayCount(uniformVariable.arrayCount);
        for (int i = 0; i < modelCount; ++i) {
            for (int j = 0; j < uniformVariable.arrayCount; ++j) {
                memoryUtility->createBuffer(uniformVariable.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            uniformVariable.buffers[i][j], uniformVariable.memorys[i][j]);
            }
        }
    }

    for (StaticUniformVariable &staticUniformVariable : staticUniformVariables[pipelineIndex]){
        staticUniformVariable.resizeArrayCount(staticUniformVariable.arrayCount);
        for (int i = 0; i < staticUniformVariable.arrayCount; ++i) {
            memoryUtility->createBuffer(staticUniformVariable.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        staticUniformVariable.buffers[i], staticUniformVariable.memorys[i]);
        }
    }

}

void GraphicsPipelineManager::createDescriptorPool() {

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = static_cast<uint32_t>(scene->entities.size()) * 2;

    if (vkCreateDescriptorPool(vulkanManager->device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Descriptor pool");
    }

}

void GraphicsPipelineManager::createDescriptorSet(uint32_t pipelineIndex) {

    std::vector<std::vector<VkDescriptorBufferInfo>> staticBufferInfos{};
    staticBufferInfos.resize(staticUniformVariables[pipelineIndex].size());

    for (int i = 0; i < staticBufferInfos.size(); ++i) {
        staticBufferInfos[i].resize(staticUniformVariables[pipelineIndex][i].arrayCount);
        for (int j = 0; j < staticBufferInfos[i].size(); ++j) {
            staticBufferInfos[i][j].buffer = staticUniformVariables[pipelineIndex][i].buffers[j];
            staticBufferInfos[i][j].offset = 0;
            staticBufferInfos[i][j].range = staticUniformVariables[pipelineIndex][i].size;
        }
    }

    std::vector<VkDescriptorImageInfo> staticImageInfos{};
    staticImageInfos.resize(staticTextureSamplers[pipelineIndex].size());

    for (int i = 0; i < staticImageInfos.size(); ++i) {
        staticImageInfos[i].imageView = staticTextureSamplers[pipelineIndex][i].imageView;
        staticImageInfos[i].sampler = staticTextureSamplers[pipelineIndex][i].sampler;
        staticImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();

    descriptorSets[pipelineIndex].resize(descriptorSetLayouts.size());
    VkResult result = vkAllocateDescriptorSets (vulkanManager->device, &allocInfo, descriptorSets[pipelineIndex].data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error ("Failed to allocate descriptor set!\n");
    }

    for (int i = 0; i < descriptorSetLayouts.size(); ++i) {

        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos{};
        bufferInfos.resize(totalUniformArrayCount);

        for (int j = 0; j < uniformVariables[pipelineIndex].size(); ++j) {
            bufferInfos[j].resize(uniformVariables[pipelineIndex][j].arrayCount);
            for (int k = 0; k < bufferInfos[j].size(); ++k) {
                bufferInfos[j][k].buffer = uniformVariables[pipelineIndex][j].buffers[i][k];
                bufferInfos[j][k].offset = 0;
                bufferInfos[j][k].range = uniformVariables[pipelineIndex][j].size;
            }
        }

        std::vector<VkDescriptorImageInfo> imageInfos{};
        imageInfos.resize(textureSamplers[pipelineIndex].size());

        for (int j = 0; j < imageInfos.size(); ++j) {
            imageInfos[j].imageView = textureSamplers[pipelineIndex][j].imageViews[i];
            imageInfos[j].sampler = textureSamplers[pipelineIndex][j].samplers[i];
            imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites = {};

        for (int j = 0 ; j < staticBufferInfos.size() ; ++j){
            for (int k = 0; k < staticBufferInfos[j].size(); ++k) {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][i];
                descriptorWrite.dstBinding = staticUniformVariables[pipelineIndex][j].binding;
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
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][i];
                descriptorWrite.dstBinding = uniformVariables[pipelineIndex][j].binding;
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
            descriptorWrite.dstSet = descriptorSets[pipelineIndex][i];
            descriptorWrite.dstBinding = staticTextureSamplers[pipelineIndex][j].binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &staticImageInfos[j];
            descriptorWrites.push_back(descriptorWrite);
        }

        for (int j = 0; j < imageInfos.size(); ++j) {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[pipelineIndex][i];
            descriptorWrite.dstBinding = textureSamplers[pipelineIndex][j].binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfos[j];
            descriptorWrites.push_back(descriptorWrite);
        }

        vkUpdateDescriptorSets (vulkanManager->device, static_cast<uint32_t>(descriptorWrites.size ()), descriptorWrites.data (), 0, nullptr);

    }

}

void GraphicsPipelineManager::createVertexInputInfo(VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo,
                                                    std::vector<VkVertexInputBindingDescription> &bindings,
                                                    std::vector<VkVertexInputAttributeDescription> &attributes) {

    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = bindings.data();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributes.data();

}

void
GraphicsPipelineManager::createInputAssemblyInfo(VkPipelineInputAssemblyStateCreateInfo &inputAssemblyStateCreateInfo) {

    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

}

void GraphicsPipelineManager::createViewportInfo(VkPipelineViewportStateCreateInfo &viewportStateCreateInfo) {

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkanManager->swapChainExtent.width;
    viewport.height = (float)vulkanManager->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = { 0,0 };
    scissor.extent = vulkanManager->swapChainExtent;

    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

}

void
GraphicsPipelineManager::createRasterizationInfo(VkPipelineRasterizationStateCreateInfo &rasterizationStateCreateInfo) {

    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

}

void GraphicsPipelineManager::createMultisampleInfo(VkPipelineMultisampleStateCreateInfo &multisampleStateCreateInfo) {

    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = ((vulkanManager->samples == VK_SAMPLE_COUNT_1_BIT) ? VK_FALSE : VK_TRUE);
    multisampleStateCreateInfo.rasterizationSamples = vulkanManager->samples;
    multisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
    multisampleStateCreateInfo.pSampleMask = nullptr; // Optional
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

}

void GraphicsPipelineManager::createBlendInfo(VkPipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo,
                                              PipelineTemplate & pipelineTemplate) {

    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(pipelineTemplate.blendAttachments.size());
    colorBlendStateCreateInfo.pAttachments = pipelineTemplate.blendAttachments.data();
    colorBlendStateCreateInfo.logicOpEnable = pipelineTemplate.blendInfo.logicOpEnable;

}

void GraphicsPipelineManager::createDynamicStateInfo(VkPipelineDynamicStateCreateInfo &dynamicStateCreateInfo) {

//    VkDynamicState dynamicStates[] = {
//
//    };
//
//    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicStateInfo.dynamicStateCount = 0;
//    dynamicStateInfo.pDynamicStates = dynamicStates;


}

void
GraphicsPipelineManager::createDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo &depthStencilStateCreateInfo) {

    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
//    depthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
//    depthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
//    depthStencilStateCreateInfo.front = {}; // Optional
//    depthStencilStateCreateInfo.back = {}; // Optional

}

void GraphicsPipelineManager::cleanUp() {

    for (auto &descriptorSetLayout : descriptorSetLayouts){
        vkDestroyDescriptorSetLayout(vulkanManager->device, descriptorSetLayout, nullptr);
    }
    descriptorSetLayouts.clear();

    for (int i = 0; i < pipelineLayouts.size(); ++i) {
        for (int j = 0; j < pipelineLayouts[i].size(); ++j) {
            vkDestroyPipelineLayout(vulkanManager->device, pipelineLayouts[i][j], nullptr);
        }
    }
    pipelineLayouts.clear();

    for (auto pipeline : pipelines){
        vkDestroyPipeline(vulkanManager->device, pipeline, nullptr);
    }
    pipelines.clear();

}

void GraphicsPipelineManager::cleanUpFinal() {

    //cleanUp();
    for (auto &descriptorSetLayout : descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(vulkanManager->device, descriptorSetLayout, nullptr);
    }
    descriptorSetLayouts.clear();

    for (int i = 0; i < pipelineTemplates.size(); ++i) {
        for (UniformVariable &uniformVariable : uniformVariables[i]) {
            for (int i = 0; i < scene->entities.size(); i++) {
                for (int j = 0; j < uniformVariable.buffers[i].size(); ++j) {
                    vkDestroyBuffer(vulkanManager->device, uniformVariable.buffers[i][j], nullptr);
                    vkFreeMemory(vulkanManager->device, uniformVariable.memorys[i][j], nullptr);
                }
            }
            uniformVariable.memorys.clear();
            uniformVariable.buffers.clear();
        }


        for (StaticUniformVariable &staticUniformVariable : staticUniformVariables[i]) {
            for (int i = 0; i < staticUniformVariable.arrayCount; ++i) {
                vkDestroyBuffer(vulkanManager->device, staticUniformVariable.buffers[i], nullptr);
                vkFreeMemory(vulkanManager->device, staticUniformVariable.memorys[i], nullptr);
            }
            staticUniformVariable.memorys.clear();
            staticUniformVariable.buffers.clear();
        }


        for (TextureSampler &textureSampler : textureSamplers[i]) {
            textureSampler.imageViews.clear();
            textureSampler.samplers.clear();
        }

        for (StaticTextureSampler &staticTextureSampler : staticTextureSamplers[i]) {
            staticTextureSampler.imageView = VK_NULL_HANDLE;
            staticTextureSampler.sampler = VK_NULL_HANDLE;
        }

    }

    uniformVariables.clear();
    staticUniformVariables.clear();
    uniformVariables.clear();
    staticUniformVariables.clear();

    vkDestroyDescriptorPool(vulkanManager->device, descriptorPool, nullptr);
    descriptorPool = VK_NULL_HANDLE;

}
