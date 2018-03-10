#include "InstanceManager.h"

void InstanceManager::createInstance(const char* applicationName, const std::vector<const char*> extensionNames, std::vector<const char*> validationLayerNames) {

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = applicationName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    applicationInfo.pEngineName = "NO ENGINE";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    applicationInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
    createInfo.ppEnabledExtensionNames = extensionNames.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayerNames.size());
    createInfo.ppEnabledLayerNames = validationLayerNames.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanManager->instance);

    if (result != VK_SUCCESS){
        throw std::runtime_error("Failed to create vulkan instance!\n");
    }

}

void InstanceManager::cleanup() {
    vkDestroyInstance(vulkanManager->instance, nullptr);
}


