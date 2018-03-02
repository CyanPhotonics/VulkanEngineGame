#include "RenderPassManager.h"

void RenderPassManager::createRenderPassesForSwapChain() {

    VkAttachmentDescription swapChainAttachmentDesc = {};
    swapChainAttachmentDesc.format = vulkanManager->swapChainImageFormat;
    swapChainAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    swapChainAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    swapChainAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    swapChainAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapChainAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    swapChainAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    swapChainAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference swapChainAttachmentRef = {};
    swapChainAttachmentRef.attachment = 0;
    swapChainAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachmentDesc = {};
    depthAttachmentDesc.format = vulkanManager->depthFormat;
    depthAttachmentDesc.samples = vulkanManager->samples;
    depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription multiAttachmentDesc = {};
    multiAttachmentDesc.format = vulkanManager->swapChainImageFormat;
    multiAttachmentDesc.samples = vulkanManager->samples;
    multiAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    multiAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    multiAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    multiAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    multiAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    multiAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference multiAttachmentRef = {};
    multiAttachmentRef.attachment = 2;
    multiAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    if (vulkanManager->samples != VK_SAMPLE_COUNT_1_BIT) {
        subpass.pColorAttachments = &multiAttachmentRef;
    } else {
        subpass.pColorAttachments = &swapChainAttachmentRef;
    }
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    if (vulkanManager->samples != VK_SAMPLE_COUNT_1_BIT) {
        subpass.pResolveAttachments = &swapChainAttachmentRef;
    }

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachmentsDescs = { swapChainAttachmentDesc, depthAttachmentDesc, multiAttachmentDesc };

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t> (attachmentsDescs.size ());
    renderPassInfo.pAttachments = attachmentsDescs.data ();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass (vulkanManager->device, &renderPassInfo, nullptr, &vulkanManager->renderPass) != VK_SUCCESS) {
        throw std::runtime_error ("Failed to create render pass!\n");
    }

}

void RenderPassManager::cleanUp() {

    vkDestroyRenderPass(vulkanManager->device, vulkanManager->renderPass, nullptr);
    vulkanManager->renderPass = VK_NULL_HANDLE;

}
