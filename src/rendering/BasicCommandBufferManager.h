#ifndef BASIC_COMMAND_BUFFER_MANAGER_H
#define BASIC_COMMAND_BUFFER_MANAGER_H

#include <vector>
#include <array>
#include <stdexcept>

#include "../vulkanSetup/VulkanManager.h"

#include "../testing/BasicPipeline.h"
#include "../scene/SceneManager.h"

class BasicCommandBufferManager {
private:
    VulkanManager* vulkanManager;
    void write(int i, VkFramebuffer const swapChainFrameBuffer, BasicPipeline pipeline, Scene scene);

public:
    explicit BasicCommandBufferManager(VulkanManager* vulkanManager) { this->vulkanManager = vulkanManager; }
    void createCommandBuffers(const std::vector<VkFramebuffer> &swapChainFrameBuffers, BasicPipeline &pipeline, Scene &scene);
    std::vector<VkCommandBuffer> commandBuffers = {};
    void cleanUp();


};

#endif //COMMAND_BUFFER_MANAGER_H
