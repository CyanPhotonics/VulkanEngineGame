#include "MemoryUtility.h"

void MemoryUtility::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                                 VkDeviceMemory &deviceMemory, VkDescriptorBufferInfo &descriptorBufferInfo) {

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanManager->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS){
        throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(vulkanManager->device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanManager->device, &allocateInfo, nullptr, &deviceMemory) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate memory!\n");
    }

    vkBindBufferMemory(vulkanManager->device, buffer, deviceMemory, 0);

    descriptorBufferInfo.buffer = buffer;
    descriptorBufferInfo.range = size;
    descriptorBufferInfo.offset = 0;

}

void MemoryUtility::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                                 VkDeviceMemory &deviceMemory) {

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanManager->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS){
        throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(vulkanManager->device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanManager->device, &allocateInfo, nullptr, &deviceMemory) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate memory!\n");
    }

    vkBindBufferMemory(vulkanManager->device, buffer, deviceMemory, 0);

}

void MemoryUtility::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {

    VkCommandBuffer commandBuffer = commandPoolManager->beginSingleTimeCommand(vulkanManager->transferCommandPool);

    VkBufferCopy region = {};
    region.srcOffset = 0;
    region.dstOffset = 0;
    region.size = size;
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &region);

    commandPoolManager->endSingleTimeCommand(commandBuffer, vulkanManager->transferCommandPool);

}

uint32_t MemoryUtility::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(vulkanManager->physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties){
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!\n");

}