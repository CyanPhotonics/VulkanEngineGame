#ifndef VULKAN_MANAGER_H
#define VULKAN_MANAGER_H

#include <vector>
#include <stdint-gcc.h>

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../rendering/GraphicsOptions.h"

class VulkanManager{
public:
    VkInstance instance{};

    VkPhysicalDevice physicalDevice{};
    VkDevice device{};

    uint32_t graphicsQueueIndex{};
    VkQueue graphicsQueue{};
    uint32_t presentQueueIndex{};
    VkQueue presentQueue{};
    uint32_t transferQueueIndex{};
    VkQueue transferQueue{};

    VkSwapchainKHR swapChain{};
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent{};

    VkSurfaceKHR surface{};
    GLFWwindow* window{};

    VkCommandPool graphicsCommandPool{};
    VkCommandPool transferCommandPool{};

    VkFormat depthFormat{};
    VkImage depthImage{};
    VkDeviceMemory depthMemory{};
    VkImageView depthImageView{};

    VkRenderPass renderPass{};

    std::vector<VkSemaphore> imageAvailableSemaphores{};
    VkSemaphore renderFinishedSemaphore{};

    GraphicsOptions graphicsOptions{};

    VulkanManager(){
        physicalDevice = VK_NULL_HANDLE;
        swapChainImageFormat = VK_FORMAT_UNDEFINED;
    }


};

#endif //VULKAN_MANAGER_H
