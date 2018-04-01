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

    VkAttachmentDescription storageAttachmentDesc = {};
    storageAttachmentDesc.format = vulkanManager->swapChainImageFormat;
    storageAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    storageAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachmentRef = {};
    storageAttachmentRef.attachment = 3;
    storageAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    std::array<VkSubpassDescription, 2> subpassArray{};

    subpassArray[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassArray[0].colorAttachmentCount = 1;
    if (vulkanManager->samples != VK_SAMPLE_COUNT_1_BIT) {
        subpassArray[0].pColorAttachments = &multiAttachmentRef;
    } else {
        subpassArray[0].pColorAttachments = &storageAttachmentRef;
    }
    subpassArray[0].pDepthStencilAttachment = &depthAttachmentRef;
    if (vulkanManager->samples != VK_SAMPLE_COUNT_1_BIT) {
        subpassArray[0].pResolveAttachments = &storageAttachmentRef;
    }
    //TODO fix?
    subpassArray[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassArray[1].colorAttachmentCount = 1;
    subpassArray[1].pColorAttachments = &swapChainAttachmentRef;

    std::array<VkSubpassDependency, 2> dependencyArray{};

    dependencyArray[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencyArray[0].dstSubpass = 0;
    dependencyArray[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyArray[0].srcAccessMask = 0;
    dependencyArray[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyArray[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //TODO check dependency is done right
    dependencyArray[1].srcSubpass = 0;
    dependencyArray[1].dstSubpass = 1;
    dependencyArray[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyArray[1].srcAccessMask = 0;
    dependencyArray[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyArray[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 4> attachmentsDescs = { swapChainAttachmentDesc, depthAttachmentDesc, multiAttachmentDesc, storageAttachmentDesc };

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t> (attachmentsDescs.size ());
    renderPassInfo.pAttachments = attachmentsDescs.data ();
    renderPassInfo.subpassCount = subpassArray.size();
    renderPassInfo.pSubpasses = subpassArray.data();
    renderPassInfo.dependencyCount = dependencyArray.size();
    renderPassInfo.pDependencies = dependencyArray.data();

    if (vkCreateRenderPass (vulkanManager->device, &renderPassInfo, nullptr, &vulkanManager->renderPass) != VK_SUCCESS) {
        throw std::runtime_error ("Failed to create render pass!\n");
    }

}

void RenderPassManager::cleanUp() {

    vkDestroyRenderPass(vulkanManager->device, vulkanManager->renderPass, nullptr);
    vulkanManager->renderPass = VK_NULL_HANDLE;

}
