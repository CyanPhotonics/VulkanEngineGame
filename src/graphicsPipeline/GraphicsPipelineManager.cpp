#include <fstream>
#include "GraphicsPipelineManager.h"

void GraphicsPipelineManager::initialSetup(std::vector<PipelineTemplate> &pipelineTemplates, Scene *scene) {
    this->pipelineTemplates = &pipelineTemplates;
    this->scene = scene;
    entityCount = scene->entities.size();
    pipelineCount = pipelineTemplates.size();

    uniformVariables.resize(pipelineCount);
    staticUniformVariables.resize(pipelineCount);
    textureSamplers.resize(pipelineCount);
    staticTextureSamplers.resize(pipelineCount);

//    for (uint32_t pipelineIndex = 0 ; pipelineIndex < pipelineCount ; pipelineIndex++) {
//        createUniforms(pipelineIndex, pipelineTemplates[pipelineIndex].uniforms);
//    }

    pipelines.resize(pipelineCount);
    pipelineLayouts.resize(pipelineCount);
    descriptorSets.resize(pipelineCount);
    descriptorPools.resize(pipelineCount);
    descriptorSetLayouts.resize(pipelineCount);

    std::vector<VkShaderModule> shaderModules{};

    //Constant factors

    // Viewport/scissor
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    VkViewport viewport{};
    VkRect2D scissor{};
    createViewportInfo(viewportStateCreateInfo, viewport, scissor);

    // Assembly input
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    createInputAssemblyInfo(inputAssemblyStateCreateInfo);

    //Rasterization
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    createRasterizationInfo(rasterizationStateCreateInfo);

    //Dynamic state
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    std::vector<VkDynamicState> dynamicStates{};
    createDynamicStateInfo(dynamicStateCreateInfo, dynamicStates);

    for (uint32_t pipelineIndex = 0 ; pipelineIndex < pipelineCount ; pipelineIndex++){

        createUniforms(pipelineIndex, pipelineTemplates[pipelineIndex].uniforms);

        //Shaders
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{};
        loadShaderModules(shaderStageCreateInfos, pipelineTemplates[pipelineIndex], shaderModules);

        // Vertex input
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        createVertexInputInfo(vertexInputStateCreateInfo, pipelineTemplates[pipelineIndex].vertexInputBindings,
                              pipelineTemplates[pipelineIndex].vertexInputAttributes);

        //Multisampling
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        createMultisampleInfo(multisampleStateCreateInfo, pipelineTemplates[pipelineIndex].multiSample);

        //Depth/Stencil
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
        createDepthStencilInfo(depthStencilStateCreateInfo, pipelineTemplates[pipelineIndex].depthTestEnabled, pipelineTemplates[pipelineIndex].depthWriteEnabled);

        //Blending
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
        createBlendInfo(colorBlendStateCreateInfo, pipelineTemplates[pipelineIndex]);

        //Pipeline layout
        createPipelineLayouts(pipelineLayouts[pipelineIndex], descriptorSetLayouts[pipelineIndex]);

        //Texture samplers
        setSamplerData(pipelineIndex);

        //Uniform buffers
        createUniformBuffers(pipelineIndex);

        //Desc pool
        createDescriptorPool(pipelineIndex);

        //Descriptor Set
        createDescriptorSet(pipelineIndex, descriptorSetLayouts[pipelineIndex]);

        PipelineTemplate::CreateInfoTemplate* createInfoTemplate = &pipelineTemplates[pipelineIndex].createInfoTemplate;
        VkGraphicsPipelineCreateInfo* createInfo = &createInfoTemplate->createInfo;

        createInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo->stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());

        createInfoTemplate->shaderModules = shaderModules;
        createInfoTemplate->shaderStageCreateInfos = shaderStageCreateInfos;
        createInfo->pStages = createInfoTemplate->shaderStageCreateInfos.data();

        createInfoTemplate->vertexInputStateCreateInfo = vertexInputStateCreateInfo;
        createInfo->pVertexInputState = &createInfoTemplate->vertexInputStateCreateInfo;

        createInfoTemplate->inputAssemblyStateCreateInfo = inputAssemblyStateCreateInfo;
        createInfo->pInputAssemblyState = &createInfoTemplate->inputAssemblyStateCreateInfo;

        createInfoTemplate->viewport = viewport;
        createInfoTemplate->scissor = scissor;
        createInfoTemplate->viewportStateCreateInfo = viewportStateCreateInfo;
        createInfoTemplate->viewportStateCreateInfo.pViewports = &createInfoTemplate->viewport;
        createInfoTemplate->viewportStateCreateInfo.pScissors = &createInfoTemplate->scissor;
        createInfo->pViewportState = &createInfoTemplate->viewportStateCreateInfo;

        createInfoTemplate->rasterizationStateCreateInfo = rasterizationStateCreateInfo;
        createInfo->pRasterizationState = &createInfoTemplate->rasterizationStateCreateInfo;

        createInfoTemplate->multisampleStateCreateInfo = multisampleStateCreateInfo;
        createInfo->pMultisampleState = &createInfoTemplate->multisampleStateCreateInfo;

        createInfoTemplate->depthStencilStateCreateInfo = depthStencilStateCreateInfo;
        createInfo->pDepthStencilState = &createInfoTemplate->depthStencilStateCreateInfo;

        createInfoTemplate->colorBlendStateCreateInfo = colorBlendStateCreateInfo;
        createInfo->pColorBlendState = &createInfoTemplate->colorBlendStateCreateInfo;

        createInfoTemplate->dynamicStates = dynamicStates;
        createInfoTemplate->dynamicStateCreateInfo = dynamicStateCreateInfo;
        createInfoTemplate->dynamicStateCreateInfo.pDynamicStates = createInfoTemplate->dynamicStates.data();
        createInfo->pDynamicState = &createInfoTemplate->dynamicStateCreateInfo;

        createInfo->layout = pipelineLayouts[pipelineIndex][0];
        createInfo->subpass = pipelineTemplates[pipelineIndex].subPassIndex;

        descSetLayoutBindings.clear();

    }

}

void GraphicsPipelineManager::perSwapChainSetup() {

    for (uint32_t i = 0; i < textureSamplers.size(); ++i) {
        for (TextureSampler& textureSampler : textureSamplers[i]){
            if (!textureSampler.swapChainDependent)
                continue;

            writeTextureSampler(textureSampler, i);
        }
    }

    for (uint32_t i = 0; i < staticTextureSamplers.size(); ++i) {
        for (StaticTextureSampler& staticTextureSampler : staticTextureSamplers[i]){
            if (!staticTextureSampler.swapChainDependent)
                continue;

            writeStaticTextureSampler(staticTextureSampler, i);
        }
    }

    for (uint32_t i = 0 ; i < pipelineTemplates->size() ; i++){

        (*pipelineTemplates)[i].createInfoTemplate.createInfo.renderPass = vulkanManager->renderPass;

        if (vkCreateGraphicsPipelines (vulkanManager->device, VK_NULL_HANDLE, 1, &(*pipelineTemplates)[i].createInfoTemplate.createInfo, nullptr, &pipelines[i]) != VK_SUCCESS) { //TODO make use of multiple creates at once
            throw std::runtime_error ("Failed to create Graphics pipeline");
        }

    }

}

// Methods

void GraphicsPipelineManager::createUniforms(uint32_t pipelineIndex,
                                             std::vector<PipelineTemplate::UniformTemplate> uniforms) {

    uniformVariables[pipelineIndex].clear();
    staticUniformVariables[pipelineIndex].clear();
    textureSamplers[pipelineIndex].clear();
    staticTextureSamplers[pipelineIndex].clear();

    for (PipelineTemplate::UniformTemplate uniform : uniforms){

        if (uniform.uniformType == PipelineTemplate::UniformTemplate::VARIABLE){

            addUniformVariable(pipelineIndex, uniform.bindingIndex, uniform.arrayCount, uniform.stageFlags,
                               uniform.uniformBufferSize, uniform.memoryMode);

        } else if (uniform.uniformType == PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER ||
                uniform.uniformType == PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER_SWAP_CHAIN_DEPENDENT){

            addTextureSamplerUniform(pipelineIndex, uniform.bindingIndex, uniform.arrayCount, nullptr,
                                     uniform.stageFlags, uniform.memoryMode,
                                     uniform.uniformType == PipelineTemplate::UniformTemplate::TEXTURE_SAMPLER_SWAP_CHAIN_DEPENDENT ? VK_TRUE : VK_FALSE);

        }

    }

}

void GraphicsPipelineManager::addUniformVariable(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                                 VkShaderStageFlags stageFlags, VkDeviceSize size,
                                                 PipelineTemplate::UniformTemplate::MemoryMode memoryMode) {

    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = static_cast<uint32_t>(descSetLayoutBindings.size());
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = arrayCount;
    layoutBinding.stageFlags = stageFlags;

    int poolSizeBaseIndex = static_cast<int>(poolSizes.size());
    poolSizes.resize(poolSizes.size() + scene->entities.size()); //TODO fix that this assumes that every pipeline wants to use the scene entities
    for (int i = 0; i < scene->entities.size(); ++i) {
        poolSizes[poolSizeBaseIndex + i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[poolSizeBaseIndex + i].descriptorCount = arrayCount;
    }

    if (memoryMode == PipelineTemplate::UniformTemplate::MemoryMode::STATIC){
        StaticUniformVariable staticUniformVariable{};
        staticUniformVariable.bindingIndex = bindingIndex;
        staticUniformVariable.size = size;
        staticUniformVariable.arrayCount = arrayCount;
        //totalStaticUniformArrayCount += arrayCount;
        staticUniformVariables[pipelineIndex].push_back(staticUniformVariable);
    } else {
        UniformVariable uniformVariable{};
        uniformVariable.bindingIndex = bindingIndex;
        uniformVariable.size = size;
        uniformVariable.arrayCount = arrayCount;
        //totalUniformArrayCount += arrayCount;
        uniformVariables[pipelineIndex].push_back(uniformVariable);
    }

    (*pipelineTemplates)[pipelineIndex].uniformVariableCount++;

    //uniformBufferSizes.push_back(size);

    descSetLayoutBindings.push_back(layoutBinding);

}

void GraphicsPipelineManager::addTextureSamplerUniform(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                                       VkSampler *immutableSamplers, VkShaderStageFlags stageFlags,
                                                       PipelineTemplate::UniformTemplate::MemoryMode memoryMode,
                                                       VkBool32 swapChainDependent) {

    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = static_cast<uint32_t>(descSetLayoutBindings.size());
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBinding.pImmutableSamplers = immutableSamplers;
    layoutBinding.descriptorCount = arrayCount;
    layoutBinding.stageFlags = stageFlags;

    int poolSizeBaseIndex = static_cast<int>(poolSizes.size());
    poolSizes.resize(poolSizes.size() + scene->entities.size()); //TODO fix that this assumes that every pipeline wants to use the scene entities
    for (int i = 0; i < scene->entities.size(); ++i) {
        poolSizes[poolSizeBaseIndex + i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[poolSizeBaseIndex + i].descriptorCount = arrayCount;
    }

    if (memoryMode == PipelineTemplate::UniformTemplate::STATIC){
        StaticTextureSampler staticTextureSampler{};
        staticTextureSampler.bindingIndex = bindingIndex;
        staticTextureSampler.swapChainDependent = swapChainDependent;
        staticTextureSampler.arrayCount = arrayCount;
        staticTextureSampler.resizeArrayCount(arrayCount);
        //staticTextureSampler.imageView =
        //staticTextureSampler.sampler =
        staticTextureSamplers[pipelineIndex].push_back(staticTextureSampler);
    } else {
        TextureSampler textureSampler{};
        textureSampler.bindingIndex = bindingIndex;
        textureSampler.swapChainDependent = swapChainDependent;
        textureSampler.arrayCount = arrayCount;
        textureSampler.resizeModelCount(scene->entities.size()); //TODO fix
        textureSampler.resizeArrayCount(arrayCount);
        //textureSampler.imageViews.push_back();
        //textureSampler.samplers.push_back();
        textureSamplers[pipelineIndex].push_back(textureSampler);
    }

    descSetLayoutBindings.push_back(layoutBinding);

}


void GraphicsPipelineManager::loadShaderModules(std::vector<VkPipelineShaderStageCreateInfo> &shaderStageCreateInfos,
                                                PipelineTemplate &pipelineTemplate,
                                                std::vector<VkShaderModule> &shaderModules) {

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

void GraphicsPipelineManager::createVertexInputInfo(VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo,
                                                    std::vector<VkVertexInputBindingDescription> &bindings,
                                                    std::vector<VkVertexInputAttributeDescription> &attributes) {

    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = bindings.data();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributes.data();

}


void GraphicsPipelineManager::createInputAssemblyInfo(VkPipelineInputAssemblyStateCreateInfo &inputAssemblyStateCreateInfo) {

    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

}

void GraphicsPipelineManager::createViewportInfo(VkPipelineViewportStateCreateInfo &viewportStateCreateInfo,
                                                 VkViewport &viewport, VkRect2D &scissor) {

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

void GraphicsPipelineManager::createRasterizationInfo(VkPipelineRasterizationStateCreateInfo &rasterizationStateCreateInfo) {

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

void GraphicsPipelineManager::createMultisampleInfo(VkPipelineMultisampleStateCreateInfo &multisampleStateCreateInfo,
                                                    VkBool32 &useMulitsample) {

    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    if (useMulitsample == VK_TRUE) {
        multisampleStateCreateInfo.sampleShadingEnable = ((vulkanManager->samples == VK_SAMPLE_COUNT_1_BIT) ? VK_FALSE
                                                                                                            : VK_TRUE);
        multisampleStateCreateInfo.rasterizationSamples = vulkanManager->samples;
    } else {
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }
    multisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
    multisampleStateCreateInfo.pSampleMask = nullptr; // Optional
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

}

void GraphicsPipelineManager::createDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo &depthStencilStateCreateInfo,
                                                     VkBool32 &test, VkBool32 &write) {

    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = test;
    depthStencilStateCreateInfo.depthWriteEnable = write;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
//    depthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
//    depthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
//    depthStencilStateCreateInfo.front = {}; // Optional
//    depthStencilStateCreateInfo.back = {}; // Optional

}

void GraphicsPipelineManager::createBlendInfo(VkPipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo,
                                              PipelineTemplate & pipelineTemplate) {

    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(pipelineTemplate.blendAttachments.size());
    colorBlendStateCreateInfo.pAttachments = pipelineTemplate.blendAttachments.data();
    colorBlendStateCreateInfo.logicOpEnable = pipelineTemplate.blendInfo.logicOpEnable;

}

void GraphicsPipelineManager::createDynamicStateInfo(VkPipelineDynamicStateCreateInfo &dynamicStateCreateInfo,
                                                     std::vector<VkDynamicState> &dynamicStates) {

    dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();


}

void GraphicsPipelineManager::createPipelineLayouts(std::vector<VkPipelineLayout> &pipelineLayouts,
                                                    std::vector<VkDescriptorSetLayout> &descriptorSetLayouts) {

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = descSetLayoutBindings.data();

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.flags = VkPipelineLayoutCreateFlags();
//    layoutInfo.pushConstantRangeCount = 0; // Optional
//    layoutInfo.pPushConstantRanges = 0; // Optional

    pipelineLayouts.resize(scene->entities.size()); //TODO fix these assuming that every swap chain wants to render all objects
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

void GraphicsPipelineManager::setSamplerData(uint32_t pipelineIndex) {

    switch (pipelineIndex){
        case 1:
            break;
        default:
            for (TextureSampler& textureSampler : textureSamplers[pipelineIndex]){

                for (uint32_t modelIndex = 0; modelIndex < scene->entities.size(); ++modelIndex) {

                    textureSampler.accessSampler(modelIndex) = scene->entities[modelIndex].texturedModel.texture.sampler;
                    textureSampler.accessImageView(modelIndex) = scene->entities[modelIndex].texturedModel.texture.imageView;
                    textureSampler.accessImageInfo(modelIndex).sampler = textureSampler.accessSampler(modelIndex);
                    textureSampler.accessImageInfo(modelIndex).imageView = textureSampler.accessImageView(modelIndex);
                    textureSampler.accessImageInfo(modelIndex).imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                }
                
            }
            
            break;
    }

}

void GraphicsPipelineManager::createUniformBuffers(uint32_t pipelineIndex) {

    size_t modelCount = scene->entities.size(); //TODO fix these assuming that every swap chain wants to render all objects

    for (UniformVariable &uniformVariable : uniformVariables[pipelineIndex]){
        uniformVariable.resizeModelCount(modelCount);
        uniformVariable.resizeArrayCount(uniformVariable.arrayCount);
        for (uint32_t modelIndex = 0; modelIndex < modelCount; ++modelIndex) {
            for (uint32_t arrayIndex = 0; arrayIndex < uniformVariable.arrayCount; ++arrayIndex) {
                memoryUtility->createBuffer(uniformVariable.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            uniformVariable.accessBuffer(modelIndex, arrayIndex),
                                            uniformVariable.accessMemory(modelIndex, arrayIndex),
                                            uniformVariable.accessBufferInfo(modelIndex, arrayIndex));
            }
        }
    }

    for (StaticUniformVariable &staticUniformVariable : staticUniformVariables[pipelineIndex]){
        staticUniformVariable.resizeArrayCount(staticUniformVariable.arrayCount);
        for (uint32_t arrayIndex = 0; arrayIndex < staticUniformVariable.arrayCount; ++arrayIndex) {
            memoryUtility->createBuffer(staticUniformVariable.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        staticUniformVariable.accessBuffer(arrayIndex),
                                        staticUniformVariable.accessMemory(arrayIndex),
                                        staticUniformVariable.accessBufferInfo(arrayIndex));
        }
    }

}

void GraphicsPipelineManager::createDescriptorPool(uint32_t pipelineIndex) {

//    if (pipelineTemplates[pipelineIndex].uniformVariableCount == 0)
//        return;

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = (uint32_t) std::max(poolSizes.size(), (size_t) 1);
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = static_cast<uint32_t>(scene->entities.size());

    if (vkCreateDescriptorPool(vulkanManager->device, &createInfo, nullptr, &descriptorPools[pipelineIndex]) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Descriptor pool");
    }

    poolSizes.clear();

}

void GraphicsPipelineManager::createDescriptorSet(uint32_t pipelineIndex, std::vector<VkDescriptorSetLayout> descriptorSetLayouts) {

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPools[pipelineIndex];
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();

    descriptorSets[pipelineIndex].resize(descriptorSetLayouts.size());
    VkResult result = vkAllocateDescriptorSets (vulkanManager->device, &allocInfo, descriptorSets[pipelineIndex].data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error ("Failed to allocate descriptor set ind !\n");
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites = {};

    for (uint32_t modelIndex = 0; modelIndex < descriptorSets[pipelineIndex].size(); ++modelIndex) {

        for (UniformVariable& uniformVariable : uniformVariables[pipelineIndex]){
            for (uint32_t arrayIndex = 0; arrayIndex < uniformVariable.arrayCount; ++arrayIndex) {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][modelIndex];
                descriptorWrite.dstBinding = uniformVariable.bindingIndex;
                descriptorWrite.dstArrayElement = arrayIndex;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &uniformVariable.accessBufferInfo(modelIndex, arrayIndex);
                descriptorWrites.push_back(descriptorWrite);
            }
        }

        for (StaticUniformVariable& staticUniformVariable : staticUniformVariables[pipelineIndex]){
            for (uint32_t arrayIndex = 0; arrayIndex < staticUniformVariable.arrayCount; ++arrayIndex) {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][modelIndex];
                descriptorWrite.dstBinding = staticUniformVariable.bindingIndex;
                descriptorWrite.dstArrayElement = arrayIndex;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &staticUniformVariable.accessBufferInfo(arrayIndex);
                descriptorWrites.push_back(descriptorWrite);
            }
        }

        for (TextureSampler& textureSampler : textureSamplers[pipelineIndex]){

            if (textureSampler.swapChainDependent) continue;

            for (uint32_t arrayIndex = 0; arrayIndex < textureSampler.arrayCount; ++arrayIndex) {

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][modelIndex];
                descriptorWrite.dstBinding = textureSampler.bindingIndex;
                descriptorWrite.dstArrayElement = arrayIndex;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &textureSampler.accessImageInfo(modelIndex, arrayIndex);
                descriptorWrites.push_back(descriptorWrite);

            }
        }

        for (StaticTextureSampler& staticTextureSampler : staticTextureSamplers[pipelineIndex]){

            if (staticTextureSampler.swapChainDependent) continue;

            for (uint32_t arrayIndex = 0; arrayIndex < staticTextureSampler.arrayCount; ++arrayIndex) {

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][modelIndex];
                descriptorWrite.dstBinding = staticTextureSampler.bindingIndex;
                descriptorWrite.dstArrayElement = arrayIndex;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &staticTextureSampler.accessImageInfo(arrayIndex);
                descriptorWrites.push_back(descriptorWrite);

            }
        }


    }

    vkUpdateDescriptorSets (vulkanManager->device, static_cast<uint32_t>(descriptorWrites.size ()), descriptorWrites.data (), 0, nullptr); //TODO move out so done once?


}

void GraphicsPipelineManager::writeTextureSampler(TextureSampler &textureSampler, uint32_t pipelineIndex) {

    for (int i = 0; i < descriptorSets.size(); ++i) {

        std::vector<VkWriteDescriptorSet> descriptorWrites = {};

        for (uint32_t modelIndex = 0; modelIndex < textureSampler.modelCount; ++modelIndex) {
            for (uint32_t arrayIndex = 0; arrayIndex < textureSampler.arrayCount; ++arrayIndex) {

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = descriptorSets[pipelineIndex][i];
                descriptorWrite.dstBinding = textureSampler.bindingIndex;
                descriptorWrite.dstArrayElement = arrayIndex;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &textureSampler.accessImageInfo(modelIndex, arrayIndex);
                descriptorWrites.push_back(descriptorWrite);

            }
        }

        vkUpdateDescriptorSets(vulkanManager->device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr); //TODO move out so done once?

    }

}

void GraphicsPipelineManager::writeStaticTextureSampler(StaticTextureSampler &staticTextureSampler, uint32_t pipelineIndex) {

    for (int i = 0; i < descriptorSets.size(); ++i) {

        std::vector<VkWriteDescriptorSet> descriptorWrites = {};

        for (uint32_t arrayIndex = 0; arrayIndex < staticTextureSampler.arrayCount; ++arrayIndex) {

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[pipelineIndex][i];
            descriptorWrite.dstBinding = staticTextureSampler.bindingIndex;
            descriptorWrite.dstArrayElement = arrayIndex;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &staticTextureSampler.accessImageInfo(arrayIndex);
            descriptorWrites.push_back(descriptorWrite);

        }

        vkUpdateDescriptorSets(vulkanManager->device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr); //TODO move out so done once?

    }

}

void GraphicsPipelineManager::swapChainCleanUp() {

    for (VkPipeline& pipeline : pipelines){

        vkDestroyPipeline(vulkanManager->device, pipeline, nullptr);

    }

}

void GraphicsPipelineManager::finalCleanUp() {

    for (int pipelineIndex = 0; pipelineIndex < pipelineCount; ++pipelineIndex) {

        PipelineTemplate* pipelineTemplate = & (*pipelineTemplates)[pipelineIndex];
        PipelineTemplate::CreateInfoTemplate* createInfoTemplate = &pipelineTemplate->createInfoTemplate;

        for (VkShaderModule shaderModule : createInfoTemplate->shaderModules){
            vkDestroyShaderModule (vulkanManager->device, shaderModule, nullptr);
        }
        createInfoTemplate->shaderModules.clear();

        for (auto &descriptorSetLayout : descriptorSetLayouts[pipelineIndex]){
            vkDestroyDescriptorSetLayout(vulkanManager->device, descriptorSetLayout, nullptr);
        }
        descriptorSetLayouts[pipelineIndex].clear();

        for (UniformVariable& uniformVariable : uniformVariables[pipelineIndex]){
            for (uint32_t modelCount = 0; modelCount < uniformVariable.modelCount; ++modelCount) {
                for (uint32_t arrayCount = 0; arrayCount < uniformVariable.arrayCount; ++arrayCount) {
                    vkDestroyBuffer(vulkanManager->device, uniformVariable.accessBuffer(modelCount, arrayCount), nullptr);
                    vkFreeMemory(vulkanManager->device, uniformVariable.accessMemory(modelCount, arrayCount), nullptr);
                }
            }

            uniformVariable.resizeArrayCount(0);
            uniformVariable.resizeModelCount(0);
        }

        for (StaticUniformVariable& staticUniformVariable : staticUniformVariables[pipelineIndex]){
            for (uint32_t arrayIndex = 0; arrayIndex < staticUniformVariable.modelCount; ++arrayIndex) {
                vkDestroyBuffer(vulkanManager->device, staticUniformVariable.accessBuffer(arrayIndex), nullptr);
                vkFreeMemory(vulkanManager->device, staticUniformVariable.accessMemory(arrayIndex), nullptr);
            }

            staticUniformVariable.resizeArrayCount(0);
        }

        for (TextureSampler& textureSampler : textureSamplers[pipelineIndex]){
            textureSampler.resizeArrayCount(0);
            textureSampler.resizeModelCount(0);
        }

        for (StaticTextureSampler& staticTextureSampler : staticTextureSamplers[pipelineIndex]){
            staticTextureSampler.resizeArrayCount(0);
        }

        vkDestroyDescriptorPool(vulkanManager->device, descriptorPools[pipelineIndex], nullptr);

    }

    descriptorPools.clear();
    textureSamplers.clear();
    staticTextureSamplers.clear();
    uniformVariables.clear();
    staticUniformVariables.clear();
    pipelineTemplates->clear();
    descriptorSetLayouts.clear();

    for (int i = 0; i < pipelineLayouts.size(); ++i) {
        for (int j = 0; j < pipelineLayouts[i].size(); ++j) {
            vkDestroyPipelineLayout(vulkanManager->device, pipelineLayouts[i][j], nullptr);
        }
    }
    pipelineLayouts.clear();

}
