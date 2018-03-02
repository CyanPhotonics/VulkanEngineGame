#include "DeviceManager.h"

DeviceManager::DeviceManager (VulkanManager *vulkanManager) {
	this->vulkanManager = vulkanManager;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.sampleRateShading = (vulkanManager->samples == VK_SAMPLE_COUNT_1_BIT) ? VK_FALSE : VK_TRUE;
}

void DeviceManager::pickPhysicalDevice () {

	uint32_t deviceCount;
	vkEnumeratePhysicalDevices (vulkanManager->instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error ("Failed to find device that supports vulkan");
	}

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices (vulkanManager->instance, &deviceCount, physicalDevices.data ());

	for (const auto physicalDevice : physicalDevices) {
		if (isDeviceSuitable (physicalDevice)) {
			vulkanManager->physicalDevice = physicalDevice;
			break;
		}
	}

	if (vulkanManager->physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error ("Failed to find suitable physical device!\n");
	}
	
}

void DeviceManager::createLogicalDevice (std::vector<const char*> validationLayers) {

	QueueFamilyIndices indices = findQueueFamilyIndices (vulkanManager->physicalDevice, vulkanManager->surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = indices.set();


	for (int queueFamily : uniqueQueueFamilies) {

		VkDeviceQueueCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = static_cast<uint32_t>(queueFamily);
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back (createInfo);
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t> (queueCreateInfos.size ());
	createInfo.pQueueCreateInfos = queueCreateInfos.data ();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t> (deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers.size ());
	if (!validationLayers.empty()) {
		createInfo.ppEnabledLayerNames = validationLayers.data ();
	}

	if (vkCreateDevice (vulkanManager->physicalDevice, &createInfo, nullptr, &vulkanManager->device) != VK_SUCCESS) {
		throw std::runtime_error ("Failed to create logical device");
	}

	vulkanManager->graphicsQueueIndex = static_cast<uint32_t>(indices.graphicsFamily);
	vulkanManager->presentQueueIndex = static_cast<uint32_t>(indices.presentFamily);
	vulkanManager->transferQueueIndex = static_cast<uint32_t>(indices.transferFamily);
	vkGetDeviceQueue (vulkanManager->device, static_cast<uint32_t>(indices.graphicsFamily), 0, &vulkanManager->graphicsQueue);
	vkGetDeviceQueue (vulkanManager->device, static_cast<uint32_t>(indices.presentFamily), 0, &vulkanManager->presentQueue);
    vkGetDeviceQueue (vulkanManager->device, static_cast<uint32_t >(indices.transferFamily), 0, &vulkanManager->transferQueue);

}

DeviceManager::QueueFamilyIndices DeviceManager::findQueueFamilyIndices (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

	QueueFamilyIndices indices;

	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties (physicalDevice, &queueCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyPropertiesVector (queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties (physicalDevice, &queueCount, queueFamilyPropertiesVector.data ());

	int i = 0;

	for (const auto& queueFamilyProperties : queueFamilyPropertiesVector) {

		if (queueFamilyProperties.queueCount > 0 && queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

        auto presentSupport = static_cast<VkBool32>(false);
		vkGetPhysicalDeviceSurfaceSupportKHR (physicalDevice, static_cast<uint32_t>(i), surface, &presentSupport);

		if (queueFamilyProperties.queueCount > 0 && presentSupport) {
			if (indices.presentFamily == -1 || i != indices.graphicsFamily) {
				indices.presentFamily = i;
			}
		}

        if (queueFamilyProperties.queueCount > 0 && queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if(indices.transferFamily == -1 || (i != indices.graphicsFamily && i != indices.presentFamily)) {
                indices.transferFamily = i;
            }
        }

//        if (indices.allFound ()) {
//			break;
//		}

		i++;
	}

	return indices;

}

bool DeviceManager::isDeviceSuitable (VkPhysicalDevice physicalDevice) {

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties (physicalDevice, &properties);

	VkPhysicalDeviceFeatures features{};
	vkGetPhysicalDeviceFeatures (physicalDevice, &features);

    return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
           && findQueueFamilyIndices (physicalDevice, vulkanManager->surface).allFound ()
           && checkDeviceExtensionSupport(physicalDevice)
           && features.samplerAnisotropy == VK_TRUE;

}

bool DeviceManager::checkDeviceExtensionSupport (VkPhysicalDevice physicalDevice) {

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties (physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions (extensionCount);
	vkEnumerateDeviceExtensionProperties (physicalDevice, nullptr, &extensionCount, availableExtensions.data ());

	std::set<std::string> requiredExtenstions (deviceExtensions.begin (), deviceExtensions.end ());

	for (const auto& extension : availableExtensions) {
		requiredExtenstions.erase (extension.extensionName);
	}

	return requiredExtenstions.empty ();

}

void DeviceManager::cleanUp (){

	vkDestroyDevice (vulkanManager->device, nullptr);
	vulkanManager->device = VK_NULL_HANDLE;

}
