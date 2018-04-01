#ifndef SWAP_CHAIN_MANAGER_H
#define SWAP_CHAIN_MANAGER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "../vulkanSetup/VulkanManager.h"

#include "../utility/Maths.h"
#include "../vulkanSetup/DeviceManager.h"
#include "WindowManager.h"

class SwapChainManager
{
private:
	VulkanManager* vulkanManager;

    std::vector<VkImage> swapChainImages;

    struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	SwapChainSupportDetails querySwapChainSupport ();
	VkSurfaceFormatKHR chooseSwapChainSurfaceFormat (std::vector<VkSurfaceFormatKHR> surfaceFormats);
	VkPresentModeKHR chooseSwapChainPresentMode(std::vector<VkPresentModeKHR> presentModes, bool fullscreen);
	VkExtent2D chooseSwapChainExtent (VkSurfaceCapabilitiesKHR capabilities);
    VkImageView createImageView (VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

public:
	explicit SwapChainManager (VulkanManager* vulkanManager);

	void createSwapChainImagesAndViews(WindowManager manager);
	void cleanUp ();
};

#endif // !SWAP_CHAIN_MANAGER_H




