#ifndef GRAPHICS_PIPELINE_BASE_H
#define GRAPHICS_PIPELINE_BASE_H

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

enum MemoryMode{
    NON_STATIC = 0,
    STATIC = 1
};

class GraphicsPipelineBase {
private:
    VulkanManager* vulkanManager;
    MemoryUtility* memoryUtility;
    Scene* scene{};

    const char* shaderFolder = "res/shaders/";

    size_t totalUniformArrayCount = 0;
    size_t totalStaticUniformArrayCount = 0;

    //std::vector<VkDeviceSize> uniformBufferSizes{};

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    std::vector<VkShaderModule> shaderModules{};

    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    uint32_t lastBind = 0;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
    VkPipelineLayoutCreateInfo layoutInfo{};

    std::vector<VkDescriptorPoolSize> poolSizes{};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};

    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineViewportStateCreateInfo viewportInfo{};

    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};

    VkPipelineMultisampleStateCreateInfo multisampleInfo{};

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments{};
    VkPipelineColorBlendStateCreateInfo blendInfo{};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};

    void loadShaderModules(std::string &vertexFile, std::string &fragmentFile);
    std::vector<char> readFile(std::string fileName);
    VkShaderModule createShaderModule(std::vector<char> &data);
    void createVertexInputInfo(std::vector<VkVertexInputBindingDescription> &bindings, std::vector<VkVertexInputAttributeDescription> &attributes);
    void createInputAssemblyInfo();
    void createViewportInfo();
    void createRasterizationInfo();
    void createMultisampleInfo();
    void createBlendInfo();
    void createDynamicStateInfo();
    void createLayoutInfo();
    void createDepthStencilInfo();
    void createGraphicsPipeline();
    void createUniformBuffer();
    void createDescriptorPool();
    void createDescriptorSet();
protected:
    virtual void setShaderFiles(std::string &vertexFile, std::string &fragmentFile) = 0;
    virtual void setUniformVariables(size_t pointLightCount) = 0;
    virtual void setVertexInputBindings(std::vector<VkVertexInputBindingDescription> &bindings) = 0;
    virtual void setVertexInputAttributes(std::vector<VkVertexInputAttributeDescription> &attributes) = 0;
    virtual void setBlendAttachments(std::vector<VkPipelineColorBlendAttachmentState> &blendAttachments) = 0;
    virtual void setBlendInfo(VkPipelineColorBlendStateCreateInfo &blendInfo) = 0;
    void addUniformVariable(uint32_t arrayCount, VkShaderStageFlags stageFlags, VkDeviceSize size,
                            MemoryMode memoryMode = NON_STATIC);
    void addTextureSamplerUniform(uint32_t arrayCount, VkSampler *immutableSamplers,
                                      VkShaderStageFlags stageFlags, MemoryMode memoryMode = NON_STATIC);

    std::vector<std::pair<VkImageView, VkSampler>> imageViewSamplePairs{};
public:
    explicit GraphicsPipelineBase(VulkanManager* vulkanManager, MemoryUtility* memoryUtility) {
        this->vulkanManager = vulkanManager;
        this->memoryUtility = memoryUtility;
    }

    VkDescriptorPool descriptorPool{};
    std::vector<VkDescriptorSet> descriptorSets{};

    std::vector<UniformVariable> uniformVariables{};
    std::vector<StaticUniformVariable> staticUniformVariables{};
    std::vector<TextureSampler> textureSamplers{};
    std::vector<StaticTextureSampler> staticTextureSamplers{};

    VkPipeline pipeline{};
    std::vector<VkPipelineLayout> pipelineLayouts{};

    void setup();
    void firstTimeOnlySetup(Scene *scene);
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

#endif //GRAPHICS_PIPELINE_BASE_H
