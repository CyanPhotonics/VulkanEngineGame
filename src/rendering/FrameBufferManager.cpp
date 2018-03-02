#include "FrameBufferManager.h"

void FrameBufferManager::createFrameBuffersWithSwapChain(std::vector<VkImageView> extraAttachments) {

    frameBuffers.resize(vulkanManager->swapChainImageViews.size());

    for (int i = 0 ; i < vulkanManager->swapChainImageViews.size() ; i++){

        std::vector<VkImageView> attachments = {vulkanManager->swapChainImageViews[i], vulkanManager->depthImageView};
        attachments = CollectionUtility::combine(attachments, extraAttachments);

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = vulkanManager->renderPass;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.layers = 1;
        createInfo.width = vulkanManager->swapChainExtent.width;
        createInfo.height = vulkanManager->swapChainExtent.height;

        if(vkCreateFramebuffer(vulkanManager->device, &createInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create frame buffer!\n");
        }

    }

}

const std::vector<VkFramebuffer> FrameBufferManager::getFrameBuffers() const {
    return frameBuffers;
}

void FrameBufferManager::cleanUp() {
    for (VkFramebuffer &frameBuffer : frameBuffers){
        vkDestroyFramebuffer(vulkanManager->device, frameBuffer, nullptr);
        frameBuffer = VK_NULL_HANDLE;
    }
}
