#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <vulkan\vulkan.h>

#include <vector>
#include <set>

#include <iostream>

#include "VulkanManager.h"

class DeviceManager{

private:
	VulkanManager* vulkanManager;

    const float queuePriority = 1.0f;

	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkPhysicalDeviceFeatures deviceFeatures = {};

	bool isDeviceSuitable (VkPhysicalDevice physicalDevice);
	bool checkDeviceExtensionSupport (VkPhysicalDevice physicalDevice);

public:
	explicit DeviceManager (VulkanManager* vulkanManager);

	void pickPhysicalDevice ();
	void createLogicalDevice (std::vector<const char*> validationLayers);

	struct QueueFamilyIndices {

		int graphicsFamily = -1;
		int presentFamily = -1;
        int transferFamily = -1;

		bool allFound () {
			return graphicsFamily >= 0 && presentFamily >= 0 && transferFamily >= 0;
		}

        std::set<int> set(){
            return {graphicsFamily, presentFamily, transferFamily};
        }

	};
	static QueueFamilyIndices findQueueFamilyIndices (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	void cleanUp ();

};

#endif // !DEVICE_MANAGER_H

