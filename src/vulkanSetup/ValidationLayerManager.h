#ifndef VALIDATION_LAYER_MANAGER_H
#define VALIDATION_LAYER_MANAGER_H


#include <vector>
#include <iostream>
#include <cstring>

#include "VulkanManager.h"

class ValidationLayerManager {

private:
	VulkanManager* vulkanManager;

	VkDebugReportCallbackEXT callback;

	bool validationLayersSupported (std::vector<const char*> validationLayers);

public:
	explicit ValidationLayerManager (VulkanManager *vulkanManager);

	void validateLayerSupport(std::vector<const char *> &validationLayers, std::vector<const char *> &vector);
	void setup (std::vector<const char*> validationLayers);
	void cleanUp ();

};

#endif // !VALIDATION_LAYER_MANAGER_H
