#ifndef UNIFORM_VARIABLES_H
#define UNIFORM_VARIABLES_H

#include <vector>
#include <sstream>

#include <vulkan/vulkan.hpp>

class UniformVariable {
private:
    std::vector<std::vector<VkBuffer>> buffers{};
    std::vector<std::vector<VkDeviceMemory>> memorys{};
    std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos{};
public:
    uint32_t bindingIndex = 0;
    size_t modelCount = 0;
    size_t arrayCount = 1;
    VkDeviceSize size = 0;

    /*inline*/ VkBuffer& accessBuffer(uint32_t modelIndex, uint32_t arrayIndex);
    /*inline*/ VkBuffer& accessBuffer(uint32_t modelIndex);

    /*inline*/ VkDeviceMemory& accessMemory(uint32_t modelIndex, uint32_t arrayIndex);
    /*inline*/ VkDeviceMemory& accessMemory(uint32_t modelIndex);

    /*inline*/ VkDescriptorBufferInfo& accessBufferInfo(uint32_t modelIndex, uint32_t arrayIndex);
    /*inline*/ VkDescriptorBufferInfo& accessBufferInfo(uint32_t modelIndex);

    void resizeModelCount(size_t size) {
        modelCount = size;
        buffers.resize(size);
        memorys.resize(size);
        bufferInfos.resize(size);
    }

    void resizeArrayCount(size_t size) {
        for (int i = 0; i < buffers.size(); ++i) {
            buffers[i].resize(size);
            memorys[i].resize(size);
            bufferInfos[i].resize(size);
        }
    }
};

class StaticUniformVariable{
private:
    std::vector<VkBuffer> buffers{};
    std::vector<VkDeviceMemory> memorys{};
    std::vector<VkDescriptorBufferInfo> bufferInfos{};
public:
    uint32_t bindingIndex = 0;
    size_t modelCount = 0;
    size_t arrayCount = 1;
    VkDeviceSize size = 0;

    /*inline*/ VkBuffer& accessBuffer(uint32_t arrayIndex);
    /*inline*/ VkBuffer& accessBuffer();

    /*inline*/ VkDeviceMemory& accessMemory(uint32_t arrayIndex);
    /*inline*/ VkDeviceMemory& accessMemory();

    /*inline*/ VkDescriptorBufferInfo& accessBufferInfo(uint32_t arrayIndex);
    /*inline*/ VkDescriptorBufferInfo& accessBufferInfo();

    void resizeArrayCount(size_t size){
        modelCount = size;
        buffers.resize(size);
        memorys.resize(size);
        bufferInfos.resize(size);
    }
};


#endif //UNIFORM_VARIABLES_H
