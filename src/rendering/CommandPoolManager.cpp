#include "CommandPoolManager.h"

void CommandPoolManager::createCommandPools() {

    VkCommandPoolCreateInfo graphicsPoolCreateInfo = {};
    graphicsPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    graphicsPoolCreateInfo.queueFamilyIndex = vulkanManager->graphicsQueueIndex;

    if(vkCreateCommandPool(vulkanManager->device, &graphicsPoolCreateInfo, nullptr, &vulkanManager->graphicsCommandPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create command pool");
    }

    if (vulkanManager->graphicsQueueIndex != vulkanManager->transferQueueIndex){

        VkCommandPoolCreateInfo transferPoolCreateInfo = {};
        transferPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transferPoolCreateInfo.queueFamilyIndex = vulkanManager->transferQueueIndex;

        if(vkCreateCommandPool(vulkanManager->device, &transferPoolCreateInfo, nullptr, &vulkanManager->transferCommandPool) != VK_SUCCESS){
            throw std::runtime_error("Failed to create command pool");
        }

    } else {

        vulkanManager->transferCommandPool = vulkanManager->graphicsCommandPool;

    }

}

VkCommandBuffer CommandPoolManager::beginSingleTimeCommand(VkCommandPool commandPool) {

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = commandPool;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkanManager->device, &allocateInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;

}

void CommandPoolManager::endSingleTimeCommand(VkCommandBuffer commandBuffer, VkCommandPool commandPool) {

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkQueue queue{};
    if (commandPool == vulkanManager->graphicsCommandPool){
        queue = vulkanManager->graphicsQueue;
    } else if (commandPool == vulkanManager->transferCommandPool){
        queue = vulkanManager->transferQueue;
    }

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(vulkanManager->device, commandPool, 1, &commandBuffer);

}

void CommandPoolManager::cleanUp() {

    vkDestroyCommandPool(vulkanManager->device, vulkanManager->graphicsCommandPool, nullptr);
    vulkanManager->graphicsCommandPool = VK_NULL_HANDLE;
    if (vulkanManager->graphicsQueueIndex != vulkanManager->transferQueueIndex){
        vkDestroyCommandPool(vulkanManager->device, vulkanManager->transferCommandPool, nullptr);
        vulkanManager->transferCommandPool = VK_NULL_HANDLE;
    }

}
