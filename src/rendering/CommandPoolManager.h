#ifndef COMMAND_POOL_MANAGER_H
#define COMMAND_POOL_MANAGER_H

#include <stdexcept>

#include "../vulkanSetup/VulkanManager.h"

class CommandPoolManager {
private:
    VulkanManager* vulkanManager;
public:
    explicit CommandPoolManager(VulkanManager* vulkanManager){ this->vulkanManager = vulkanManager; }

    void createCommandPools();

    VkCommandBuffer beginSingleTimeCommand(VkCommandPool commandPool);
    void endSingleTimeCommand(VkCommandBuffer commandBuffer, VkCommandPool commandPool);

    void cleanUp();
};


#endif //COMMAND_POOL_MANAGER_H
