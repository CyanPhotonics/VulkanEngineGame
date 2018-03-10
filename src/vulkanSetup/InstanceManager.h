#ifndef INSTANCE_MANAGER_H
#define INSTANCE_MANAGER_H

#include <vector>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "VulkanManager.h"

class InstanceManager {
private:
    VulkanManager* vulkanManager;
public:
    explicit InstanceManager(VulkanManager* vulkanManager){ this->vulkanManager = vulkanManager; }

    void createInstance(const char* applicationName, const std::vector<const char*> extensionNames, std::vector<const char*> validationLayerNames);

    void cleanup();
};

#endif //INSTANCE_MANAGER_H
