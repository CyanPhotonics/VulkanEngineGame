#include <thread>
#include <iostream>
#include "BasicCommandBufferManager.h"
#include "../presentation/WindowManager.h"
#include "../utility/DebugUtility.h"

void test(std::string s) {

    std::cout << "Thread says: " << s << std::endl;

}

void
BasicCommandBufferManager::createCommandBuffers(const std::vector<VkFramebuffer> swapChainFrameBuffers,
                                                GraphicsPipelineManager graphicsPipelineManager, Scene scene, WindowManager windowManager) {

    commandBuffers.resize(vulkanManager->swapChainImageViews.size());

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = vulkanManager->graphicsCommandPool;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(vulkanManager->device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate command buffers");
    }

    std::thread threads[3];

    for (int i = 0; i < commandBuffers.size(); ++i) {
        //threads[i] = std::thread(&BasicCommandBufferManager::write, this, i, swapChainFrameBuffers[i], pipeline, scene); //TODO fix? need multiple command pools, one per image, any real benefit for now?
        write(i, swapChainFrameBuffers[i], graphicsPipelineManager, scene, windowManager);
    }

    for (int j = 0; j < 3; ++j) {
        //threads[j].join();
    }

}

void BasicCommandBufferManager::write(int i, VkFramebuffer_T *const &swapChainFrameBuffer, GraphicsPipelineManager graphicsPipelineManager, Scene scene,
                                      WindowManager windowManager) {

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = vulkanManager->renderPass;
    renderPassBeginInfo.framebuffer = swapChainFrameBuffer;
    renderPassBeginInfo.renderArea.offset = {0,0};
    renderPassBeginInfo.renderArea.extent = vulkanManager->swapChainExtent;

    float value = 0.15;

    std::array<VkClearValue, 4> clearValues = {};
    clearValues[0].color = {value,value,value,1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    clearValues[2].color = {value,value,value,1.0f};
    clearValues[3].color = {0,0,0,1.0f};

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelines[0]);

    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = windowManager.getWindowWidth() / 2.0f;
    viewport.height = windowManager.getWindowHeight() / 2.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = VkExtent2D{static_cast<uint32_t>(vulkanManager->swapChainExtent.width / 2.0f),
                                static_cast<uint32_t>(vulkanManager->swapChainExtent.height / 2.0f)};
    scissor.offset = {0,0};

    vkCmdSetViewport(commandBuffers[i],0,1,&viewport);
    vkCmdSetScissor(commandBuffers[i],0,1,&scissor);

    uint32_t j = 0;
    for (Entity entity : scene.entities) {

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelineLayouts[0][j], 0, 1, &graphicsPipelineManager.descriptorSets[0][j], 0, nullptr);

        VkBuffer vertexBuffers[] = {entity.texturedModel.model.vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[i], entity.texturedModel.model.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffers[i], entity.texturedModel.model.indexCount, 1, 0, 0, 0);

        ++j;

    }

    //Subpass #2

    viewport.width = windowManager.getWindowWidth();
    viewport.height = windowManager.getWindowHeight();
    scissor.extent = vulkanManager->swapChainExtent;
    vkCmdSetViewport(commandBuffers[i],0,1,&viewport);
    vkCmdSetScissor(commandBuffers[i],0,1,&scissor);

    vkCmdNextSubpass(commandBuffers[i], VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelines[1]);

    vkCmdSetViewport(commandBuffers[i],0,1,&viewport);

    vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelineLayouts[1][0], 0, 1, &graphicsPipelineManager.descriptorSets[1][0],0,
                            nullptr);

    vkCmdDraw(commandBuffers[i],6,1,0,0);

    vkCmdEndRenderPass(commandBuffers[i]);

    DebugUtility::VkSuccess(vkEndCommandBuffer(commandBuffers[i]), "Failed to record command buffer!");

}

void BasicCommandBufferManager::cleanUp() {

    if (commandBuffers.empty()) return;

    vkFreeCommandBuffers(vulkanManager->device, vulkanManager->graphicsCommandPool,
                         static_cast<uint32_t>(vulkanManager->swapChainImageViews.size()), commandBuffers.data());

    commandBuffers.clear();

}
