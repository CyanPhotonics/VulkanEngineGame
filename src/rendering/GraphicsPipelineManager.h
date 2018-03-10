//TODO sort out arrays being handled for some uniforms

#ifndef GRAPHICS_PIPELINE_MANAGER_H
#define GRAPHICS_PIPELINE_MANAGER_H

#include "../vulkanSetup/VulkanManager.h"

#include "../utility/MemoryUtility.h"
#include "../scene/SceneManager.h"

#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <stdexcept>
#include <tuple>

#include <glm/glm.hpp>

struct UniformVariable;
struct StaticUniformVariable;
struct TextureSampler;
struct StaticTextureSampler;

struct PipelineTemplate{

    uint32_t subpass;

    std::string vertexShaderFile = "", fragmentShaderFile = "";

    struct UniformTemplate{

        enum UniformType {
            VARIABLE = 0,
            TEXTURE_SAMPLER = 1
        };

        enum MemoryMode{
            NON_STATIC = 0,
            STATIC = 1
        };

        uint32_t index;
        VkShaderStageFlags stageFlags;
        VkDeviceSize size;
        uint32_t arrayCount;
        UniformType uniformType;
        MemoryMode memoryMode;

        UniformTemplate(uint32_t index, uint32_t arrayCount, VkDeviceSize size, VkShaderStageFlags stageFlags, UniformType uniformType, MemoryMode memoryMode){
            this->index = index;
            this->arrayCount = arrayCount;
            this->size = size;
            this->stageFlags = stageFlags;
            this->uniformType = uniformType;
            this->memoryMode = memoryMode;
        }

    };

    std::vector<UniformTemplate> uniforms{};

    std::vector<VkVertexInputBindingDescription> vertexInputBindings{};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes{};

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments{};
    VkPipelineColorBlendStateCreateInfo blendInfo{};

};

class GraphicsPipelineManager {
private:
    VulkanManager* vulkanManager;
    MemoryUtility* memoryUtility;
    Scene* scene{};

    const char* shaderFolder = "res/shaders/";

    std::vector<PipelineTemplate> pipelineTemplates{};

    size_t totalUniformArrayCount = 0;
    size_t totalStaticUniformArrayCount = 0;

    //std::vector<VkDeviceSize> uniformBufferSizes{};

    //std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    std::vector<VkShaderModule> shaderModules{};

    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    uint32_t lastBind = 0;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
    VkPipelineLayoutCreateInfo layoutInfo{};

    std::vector<VkDescriptorPoolSize> poolSizes{};
    VkViewport viewport{};
    VkRect2D scissor{};

    void loadShaderModules(std::vector<VkPipelineShaderStageCreateInfo> &shaderStageCreateInfos,
                               PipelineTemplate &pipelineTemplate);
    std::vector<char> readFile(std::string fileName);
    VkShaderModule createShaderModule(std::vector<char> &data);
    void createVertexInputInfo(VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo,
                                   std::vector<VkVertexInputBindingDescription> &bindings,
                                   std::vector<VkVertexInputAttributeDescription> &attributes);
    void createInputAssemblyInfo(VkPipelineInputAssemblyStateCreateInfo &inputAssemblyStateCreateInfo);
    void createViewportInfo(VkPipelineViewportStateCreateInfo &viewportStateCreateInfo);
    void createRasterizationInfo(VkPipelineRasterizationStateCreateInfo &rasterizationStateCreateInfo);
    void createMultisampleInfo(VkPipelineMultisampleStateCreateInfo &multisampleStateCreateInfo);
    void createBlendInfo(VkPipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo,
                             PipelineTemplate & pipelineTemplate);
    void createDynamicStateInfo(VkPipelineDynamicStateCreateInfo &dynamicStateCreateInfo);
    void createLayoutInfo(std::vector<VkPipelineLayout> &pipelineLayouts);
    void createDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo &depthStencilStateCreateInfo);
    void createUniformBuffer(uint32_t pipelineIndex);
    void createDescriptorPool();
    void createDescriptorSet(uint32_t pipelineIndex);

    void createUniforms(uint32_t pipelineIndex,
                            std::vector<PipelineTemplate::UniformTemplate> uniforms);
    void addUniformVariable(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                VkShaderStageFlags stageFlags, VkDeviceSize size,
                                PipelineTemplate::UniformTemplate::MemoryMode memoryMode);
    void addTextureSamplerUniform(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                      VkSampler *immutableSamplers, VkShaderStageFlags stageFlags,
                                      PipelineTemplate::UniformTemplate::MemoryMode memoryMode);

    std::vector<std::pair<VkImageView, VkSampler>> imageViewSamplePairs{};
public:
    explicit GraphicsPipelineManager(VulkanManager* vulkanManager, MemoryUtility* memoryUtility) {
        this->vulkanManager = vulkanManager;
        this->memoryUtility = memoryUtility;
    }

    VkDescriptorPool descriptorPool{};
    std::vector<std::vector<VkDescriptorSet>> descriptorSets{};

    std::vector<VkPipeline> pipelines{};

    std::vector<std::vector<UniformVariable>> uniformVariables{};
    std::vector<std::vector<StaticUniformVariable>> staticUniformVariables{};
    std::vector<std::vector<TextureSampler>> textureSamplers{};
    std::vector<std::vector<StaticTextureSampler>> staticTextureSamplers{};

    //VkPipeline pipeline{};
   // VkPipeline pipeline1{};
    //std::vector<VkPipelineLayout> pipelineLayouts{};
    std::vector<std::vector<VkPipelineLayout>> pipelineLayouts{};

    void setup();
    void firstTimeOnlySetup(std::vector<PipelineTemplate> &pipelineTemplates, Scene *scene);
    void cleanUp();
    void cleanUpFinal();
};

struct UniformVariable{
    uint32_t binding = 0;

    std::vector<std::vector<VkBuffer>> buffers{};
    std::vector<std::vector<VkDeviceMemory>> memorys{};
//    std::vector<VkBuffer> buffers{};
//    std::vector<VkDeviceMemory> memorys{};
    VkDeviceSize size = 0;

    size_t arrayCount = 1;

    void resizeModelCount(size_t size){
        buffers.resize(size);
        memorys.resize(size);
    }

    void resizeArrayCount(size_t size){
        for (int i = 0; i < buffers.size(); ++i) {
            buffers[i].resize(size);
            memorys[i].resize(size);
        }
    }
};

struct StaticUniformVariable{
    uint32_t binding = 0;

    size_t arrayCount = 1;

    std::vector<VkBuffer> buffers{};
    std::vector<VkDeviceMemory> memorys{};
    VkDeviceSize size = 0;

    void resizeArrayCount(size_t size){
        buffers.resize(size);
        memorys.resize(size);
    }
};

struct TextureSampler{
    uint32_t binding = 0;

    std::vector<VkImageView> imageViews{};
    std::vector<VkSampler> samplers{};

    void resize(size_t size){
        imageViews.resize(size);
        samplers.resize(size);
    }
};

struct StaticTextureSampler{
    uint32_t binding = 0;

    VkImageView imageView{};
    VkSampler sampler{};
};

#endif //GRAPHICS_PIPELINE_MANAGER_H
