#include "SwapChainManager.h"
#include "WindowManager.h"

SwapChainManager::SwapChainManager (VulkanManager* vulkanManager) : vulkanManager(vulkanManager) {}

void SwapChainManager::createSwapChainImagesAndViews(WindowManager manager, VkSwapchainKHR oldSwapChain) {

	SwapChainSupportDetails swapChainSupport = querySwapChainSupport ();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat (swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapChainPresentMode(swapChainSupport.presentModes, manager.fullscreen);
	printf("Present mode: %d\n", presentMode);
	VkExtent2D extent = chooseSwapChainExtent (swapChainSupport.capabilities);

	uint32_t imageCount = Maths::clampReturn (swapChainSupport.capabilities.minImageCount + 1, swapChainSupport.capabilities.minImageCount, swapChainSupport.capabilities.maxImageCount > 0 ? swapChainSupport.capabilities.maxImageCount : std::numeric_limits<uint32_t>::max ());
    printf("Image count: %d\n", imageCount);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vulkanManager->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	DeviceManager::QueueFamilyIndices indices = DeviceManager::findQueueFamilyIndices (vulkanManager->physicalDevice, vulkanManager->surface);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = oldSwapChain;

	if (    vkCreateSwapchainKHR (vulkanManager->device, &createInfo, nullptr, &vulkanManager->swapChain) != VK_SUCCESS) {
		throw std::runtime_error ("Failed to create swapchain!\n");
	}

	vkGetSwapchainImagesKHR (vulkanManager->device, vulkanManager->swapChain, &imageCount, nullptr);
	swapChainImages.resize (imageCount);
	vkGetSwapchainImagesKHR (vulkanManager->device, vulkanManager->swapChain, &imageCount, swapChainImages.data ());

	vulkanManager->swapChainImageFormat = surfaceFormat.format;
	vulkanManager->swapChainExtent = extent;

	vulkanManager->swapChainImageViews.resize (imageCount);
	for (uint32_t i = 0; i < swapChainImages.size (); i++) {

		vulkanManager->swapChainImageViews[i] = createImageView (vulkanManager->device, swapChainImages[i], vulkanManager->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

	}

}

SwapChainManager::SwapChainSupportDetails SwapChainManager::querySwapChainSupport () {

	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR (vulkanManager->physicalDevice, vulkanManager->surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR (vulkanManager->physicalDevice, vulkanManager->surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize (formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR (vulkanManager->physicalDevice, vulkanManager->surface, &formatCount, details.formats.data ());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR (vulkanManager->physicalDevice, vulkanManager->surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize (presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR (vulkanManager->physicalDevice, vulkanManager->surface, &presentModeCount, details.presentModes.data ());
	}

	return details;

}

VkSurfaceFormatKHR SwapChainManager::chooseSwapChainSurfaceFormat (std::vector<VkSurfaceFormatKHR> surfaceFormats) {

	if (surfaceFormats.size () == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& surfaceFormat : surfaceFormats) {
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return surfaceFormat;
		}
	}

	return surfaceFormats[0];

}

VkPresentModeKHR SwapChainManager::chooseSwapChainPresentMode(std::vector<VkPresentModeKHR> presentModes, bool fullscreen) {

    if (fullscreen){
        //TODO try to find fix
        return VK_PRESENT_MODE_FIFO_KHR;
    }

	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
		else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = presentMode;
		}
	}

	return bestMode;
}

VkExtent2D SwapChainManager::chooseSwapChainExtent (VkSurfaceCapabilitiesKHR capabilities) {

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max ()) {
		return capabilities.currentExtent;
	}
	else {

		int width, height;
		glfwGetFramebufferSize (vulkanManager->window, &width, &height);

		VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		actualExtent.width = std::max (capabilities.minImageExtent.width, std::min (capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max (capabilities.minImageExtent.height, std::min (capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

}


VkImageView SwapChainManager::createImageView (VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView (device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error ("failed to create texture image view!\n");
    }

    return imageView;

}

void SwapChainManager::cleanUpImageViews(){

    for (int i = 0; i < swapChainImages.size (); i++) {
        vkDestroyImageView (vulkanManager->device, vulkanManager->swapChainImageViews[i], nullptr);
        vulkanManager->swapChainImageViews[i] = VK_NULL_HANDLE;
    }

}

void SwapChainManager::cleanUp () {

	vkDestroySwapchainKHR (vulkanManager->device, vulkanManager->swapChain, nullptr);
	vulkanManager->swapChain = VK_NULL_HANDLE;

}

void SwapChainManager::cleanUp(VkSwapchainKHR &swapChain) {

    vkDestroySwapchainKHR (vulkanManager->device, swapChain, nullptr);
    swapChain = VK_NULL_HANDLE;

}

