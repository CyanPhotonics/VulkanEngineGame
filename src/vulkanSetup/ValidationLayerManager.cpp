#include "ValidationLayerManager.h"

ValidationLayerManager::ValidationLayerManager (VulkanManager *vulkanManager) : vulkanManager(vulkanManager) {}

void ValidationLayerManager::validateLayerSupport(std::vector<const char *> validationLayers, std::vector<const char *> &extensions) {

	if (!validationLayersSupported (validationLayers)) {
		validationLayers.clear ();
	} else {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT (VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr (instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func (instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT (VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr (instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func (instance, callback, pAllocator);
	}
}

void ValidationLayerManager::setup (const std::vector<const char*> validationLayers) {

	if (validationLayers.empty()) {return;}

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT (vulkanManager->instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error ("Failed to set up debug callback!\n");
	}

}

bool ValidationLayerManager::validationLayersSupported (std::vector<const char*> validationLayers) {

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties (&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers (layerCount);
	vkEnumerateInstanceLayerProperties (&layerCount, availableLayers.data ());

	for (const char* layerName : validationLayers) {

		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {

			if (strcmp (layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}

		}

		if (!layerFound) {
			return false;
		}

	}

	return true;

}

void ValidationLayerManager::cleanUp () {

	DestroyDebugReportCallbackEXT (vulkanManager->instance, callback, nullptr);
	callback = VK_NULL_HANDLE;

}

