#ifndef BASIC_COMMAND_BUFFER_MANAGER_H
#define BASIC_COMMAND_BUFFER_MANAGER_H

#include <vector>
#include <array>
#include <stdexcept>

#include "../vulkanSetup/VulkanManager.h"

#include "../scene/SceneManager.h"
#include "../graphicsPipeline/GraphicsPipelineManager.h"
#include "../presentation/WindowManager.h"
#include "../postProcessing/PostProcessingStage.h"
#include "../utility/DebugUtility.h"

class BasicCommandBufferManager {
private:
    VulkanManager* vulkanManager;
    void
    write(int i, VkFramebuffer_T *const &swapChainFrameBuffer, GraphicsPipelineManager graphicsPipelineManager,
          Scene scene,
          WindowManager windowManager, std::vector<PostProcessingStage> &postProcessStages);

public:
    explicit BasicCommandBufferManager(VulkanManager* vulkanManager) : vulkanManager(vulkanManager) {}
    void createCommandBuffers(const std::vector<VkFramebuffer> swapChainFrameBuffers,
                              GraphicsPipelineManager graphicsPipelineManager, Scene scene, WindowManager windowManager,
                              std::vector<PostProcessingStage> &postProcessStages);
    std::vector<VkCommandBuffer> commandBuffers = {};
    void cleanUp();


};

#endif //COMMAND_BUFFER_MANAGER_H
