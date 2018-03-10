#include <thread>
#include <iostream>
#include "BasicCommandBufferManager.h"

void test(std::string s) {

    std::cout << "Thread says: " << s << std::endl;

}

void
BasicCommandBufferManager::createCommandBuffers(const std::vector<VkFramebuffer> &swapChainFrameBuffers,
                                                GraphicsPipelineManager &graphicsPipelineManager, Scene &scene) {

    commandBuffers.resize(vulkanManager->swapChainImageViews.size());

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = vulkanManager->graphicsCommandPool;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(vulkanManager->device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate command buffers");
    }

//    std::vector<std::thread> threads{};
//    threads.resizeModelCount(commandBuffers.size());

    std::thread threads[3];

    for (int i = 0; i < commandBuffers.size(); ++i) {
        //std::thread(write, i, const_cast<VkFramebuffer>(swapChainFrameBuffers[i]), pipeline, scene);
        //std::thread(test);
        //threads[i] = std::thread(&BasicCommandBufferManager::write, this, i, swapChainFrameBuffers[i], pipeline, scene); //TODO fix? need multiple command pools, one per image, any real benefit for now?
        write(i, swapChainFrameBuffers[i], graphicsPipelineManager, scene);

        //write(i,swapChainFrameBuffers[i], pipeline, scene);
    }

    for (int j = 0; j < 3; ++j) {
        //threads[j].join();
    }

//    for (auto &thread : threads) {
//        thread.join();
//    }

}

void BasicCommandBufferManager::write(int i, VkFramebuffer const swapChainFrameBuffer,
                                      GraphicsPipelineManager graphicsPipelineManager, Scene scene) {

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

    std::array<VkClearValue, 3> clearValues = {};
    clearValues[0].color = {value,value,value,1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    clearValues[2].color = {value,value,value,1.0f};

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelines[0]);

    uint32_t j = 0;
//    for (Entity entity : scene.entities) {
//
//        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelineLayouts[0][j], 0, 1, &graphicsPipelineManager.descriptorSets[0][j], 0, nullptr);
//
//        VkBuffer vertexBuffers[] = {entity.texturedModel.model.vertexBuffer};
//        VkDeviceSize offsets[] = {0};
//        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
//        vkCmdBindIndexBuffer(commandBuffers[i], entity.texturedModel.model.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//
//        vkCmdDrawIndexed(commandBuffers[i], entity.texturedModel.model.indexCount, 1, 0, 0, 0);
//
//        ++j;
//
//    }

    //Subpass #2

    vkCmdNextSubpass(commandBuffers[i], VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelines[1]);

    j = 0;
    for (Entity entity : scene.entities) {

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineManager.pipelineLayouts[1][j], 0, 1, &graphicsPipelineManager.descriptorSets[1][j], 0, nullptr);

        VkBuffer vertexBuffers[] = {entity.texturedModel.model.vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[i], entity.texturedModel.model.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffers[i], entity.texturedModel.model.indexCount, 1, 0, 0, 0);

        ++j;

    }

    vkCmdEndRenderPass(commandBuffers[i]);

    if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
        throw std::runtime_error("Failed to record command buffer!\n");
    }

}

void BasicCommandBufferManager::cleanUp() {

    if (commandBuffers.empty()) return;

    vkFreeCommandBuffers(vulkanManager->device, vulkanManager->graphicsCommandPool,
                         static_cast<uint32_t>(vulkanManager->swapChainImageViews.size()), commandBuffers.data());

    commandBuffers.clear();

}
