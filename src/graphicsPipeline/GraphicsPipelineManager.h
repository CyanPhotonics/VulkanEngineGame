#ifndef GRAPHICS_PIPELINE_MANAGER_H
#define GRAPHICS_PIPELINE_MANAGER_H

#include <fstream>

#include "UniformVariable.h"
#include "TextureSamplers.h"

#include "../vulkanSetup/VulkanManager.h"
#include "../utility/MemoryUtility.h"
#include "../scene/Scene.h"

struct UniformVariable;
struct StaticUniformVariable;
struct TextureSampler;
struct StaticTextureSampler;

struct PipelineTemplate{

    struct UniformTemplate{

        enum UniformType {
            VARIABLE = 0,
            TEXTURE_SAMPLER = 1,
            TEXTURE_SAMPLER_SWAP_CHAIN_DEPENDENT = 2,
        };

        enum MemoryMode{
            NON_STATIC = 0,
            STATIC = 1
        };

        uint32_t bindingIndex=0;
        VkShaderStageFlags stageFlags{};
        VkDeviceSize uniformBufferSize{};
        uint32_t arrayCount{};
        UniformType uniformType{};
        MemoryMode memoryMode{};

        UniformTemplate(uint32_t index, uint32_t arrayCount, VkDeviceSize size, VkShaderStageFlags stageFlags, UniformType uniformType, MemoryMode memoryMode, VkBool32 swapChainDependent = VK_FALSE){
            this->bindingIndex = index;
            this->arrayCount = arrayCount;
            this->uniformBufferSize = size;
            this->stageFlags = stageFlags;
            this->uniformType = uniformType;
            this->memoryMode = memoryMode;
        }

    };

    struct CreateInfoTemplate {

        VkGraphicsPipelineCreateInfo createInfo{};

        std::vector<VkShaderModule> shaderModules{};
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{};
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        VkViewport viewport{};
        VkRect2D scissor{};
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        std::vector<VkDynamicState> dynamicStates{};

    } createInfoTemplate{};

    uint32_t subPassIndex;

    std::string vertexShaderFile = "", fragmentShaderFile = "";

    std::vector<UniformTemplate> uniforms{};
    uint32_t uniformVariableCount = 0;

    std::vector<VkVertexInputBindingDescription> vertexInputBindings{};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes{};

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments{};
    VkPipelineColorBlendStateCreateInfo blendInfo{};

    VkBool32 depthTestEnabled = VK_TRUE;
    VkBool32 depthWriteEnabled = VK_TRUE;

    VkBool32 multiSample = VK_TRUE;

};

class GraphicsPipelineManager {
private:
    //Constants
    VulkanManager* vulkanManager;
    MemoryUtility* memoryUtility;

    const std::string shaderFolder = "res/shaders/";

    //Init data
    std::vector<PipelineTemplate> *pipelineTemplates{};
    Scene* scene{};
    size_t entityCount = 0;
    size_t pipelineCount = 0;

    std::vector<VkDescriptorPoolSize> poolSizes{};

    std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings{};
    std::vector<std::vector<VkDescriptorSetLayout>> descriptorSetLayouts{};

    std::vector<VkDescriptorPool> descriptorPools{};

    // Create Uniform objects
    void createUniforms(uint32_t pipelineIndex, std::vector<PipelineTemplate::UniformTemplate> uniforms);
    void addUniformVariable(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                            VkShaderStageFlags stageFlags, VkDeviceSize size,
                            PipelineTemplate::UniformTemplate::MemoryMode memoryMode);
    void addTextureSamplerUniform(uint32_t pipelineIndex, uint32_t bindingIndex, uint32_t arrayCount,
                                  VkSampler *immutableSamplers, VkShaderStageFlags stageFlags,
                                  PipelineTemplate::UniformTemplate::MemoryMode memoryMode, VkBool32 swapChainDependent);

    //Shader loading
    void loadShaderModules(std::vector<VkPipelineShaderStageCreateInfo> &shaderStageCreateInfos,
                               PipelineTemplate &pipelineTemplate,
                               std::vector<VkShaderModule> &shaderModules);
    std::vector<char> readFile(std::string fileName);
    VkShaderModule createShaderModule(std::vector<char> &data);

    //Vertex input loading
    void createVertexInputInfo(VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo,
                               std::vector<VkVertexInputBindingDescription> &bindings,
                               std::vector<VkVertexInputAttributeDescription> &attributes);

    //Input assembly
    void createInputAssemblyInfo(VkPipelineInputAssemblyStateCreateInfo &inputAssemblyStateCreateInfo);

    //Viewport
    void createViewportInfo(VkPipelineViewportStateCreateInfo &viewportStateCreateInfo,
                            VkViewport &viewport, VkRect2D &scissor);

    //Rasterization
    void createRasterizationInfo(VkPipelineRasterizationStateCreateInfo &rasterizationStateCreateInfo);

    //Multi Sampleing
    void createMultisampleInfo(VkPipelineMultisampleStateCreateInfo &multisampleStateCreateInfo, VkBool32 &useMulitsample);

    //Depth
    void createDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo &depthStencilStateCreateInfo, VkBool32 &test,
                                VkBool32 &write);

    //Blend
    void createBlendInfo(VkPipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo, PipelineTemplate &pipelineTemplate);

    //Dyanmic states
    void createDynamicStateInfo(VkPipelineDynamicStateCreateInfo &dynamicStateCreateInfo,
                                std::vector<VkDynamicState> &dynamicStates);

    // layout info
    void createPipelineLayouts(std::vector<VkPipelineLayout> &pipelineLayouts,
                               std::vector<VkDescriptorSetLayout> &descriptorSetLayouts);

    //sampler data
    void setSamplerData(uint32_t pipelineIndex);

    // Uniforms
    void createUniformBuffers(uint32_t pipelineIndex);

    //descriptor pool
    void createDescriptorPool(uint32_t pipelineIndex);

    //Descriptor set
    void createDescriptorSet(uint32_t pipelineIndex, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);

    void writeTextureSampler(TextureSampler& textureSampler, uint32_t pipelineIndex);
    void writeStaticTextureSampler(StaticTextureSampler& staticTextureSampler, uint32_t pipelineIndex);

public:
    explicit GraphicsPipelineManager(VulkanManager* vulkanManager, MemoryUtility* memoryUtility) {
        this->vulkanManager = vulkanManager;
        this->memoryUtility = memoryUtility;
    }

    std::vector<VkPipeline> pipelines{};
    std::vector<std::vector<VkPipelineLayout>> pipelineLayouts{};
    std::vector<std::vector<VkDescriptorSet>> descriptorSets{};

    //Vectors of unifrom/sampler data, stored per pipeline
    std::vector<std::vector<UniformVariable>> uniformVariables{};
    std::vector<std::vector<StaticUniformVariable>> staticUniformVariables{};
    std::vector<std::vector<TextureSampler>> textureSamplers{};
    std::vector<std::vector<StaticTextureSampler>> staticTextureSamplers{};

    void initialSetup(std::vector<PipelineTemplate> &pipelineTemplates, Scene *scene);
    void perSwapChainSetup();

    void swapChainCleanUp();
    void finalCleanUp();

};

#endif //GRAPHICS_PIPELINE_MANAGER_H
