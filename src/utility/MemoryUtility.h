#ifndef BUFFER_UTILITY_H
#define BUFFER_UTILITY_H

#include "../vulkanSetup/VulkanManager.h"
#include "../rendering/CommandPoolManager.h"

class MemoryUtility{
private:
    VulkanManager* vulkanManager{};
    CommandPoolManager* commandPoolManager{};
public:
    MemoryUtility(VulkanManager* vulkanManager, CommandPoolManager* commandPoolManager){ this->vulkanManager = vulkanManager;this->commandPoolManager = commandPoolManager; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                          VkDeviceMemory &deviceMemory, VkDescriptorBufferInfo &descriptorBufferInfo);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &deviceMemory);

    void copyBuffer(VkBuffer src, VkBuffer dst,VkDeviceSize size);

};

#endif //BUFFER_UTILITY_H
