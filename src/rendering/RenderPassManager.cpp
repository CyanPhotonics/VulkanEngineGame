#include "RenderPassManager.h"
#include "../utility/DebugUtility.h"

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
    depthAttachmentDesc.samples = vulkanManager->graphicsOptions.samples;
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
    multiAttachmentDesc.samples = vulkanManager->graphicsOptions.samples;
    multiAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    multiAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    multiAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    multiAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    multiAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    multiAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference multiAttachmentRef = {};
    multiAttachmentRef.attachment = 2;
    multiAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //BLur

    VkAttachmentDescription storageAttachmentDesc = {};
    storageAttachmentDesc.format = vulkanManager->swapChainImageFormat;
    storageAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    storageAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachmentRef = {};
    storageAttachmentRef.attachment = 3;
    storageAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription storageAttachment2Desc = {};
    storageAttachment2Desc.format = vulkanManager->swapChainImageFormat;
    storageAttachment2Desc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachment2Desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachment2Desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachment2Desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachment2Desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachment2Desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    storageAttachment2Desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachment2Ref = {};
    storageAttachment2Ref.attachment = 4;
    storageAttachment2Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription storageAttachment3Desc = {};
    storageAttachment3Desc.format = vulkanManager->swapChainImageFormat;
    storageAttachment3Desc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachment3Desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachment3Desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachment3Desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachment3Desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachment3Desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    storageAttachment3Desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachment3Ref = {};
    storageAttachment3Ref.attachment = 5;
    storageAttachment3Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription storageAttachment4Desc = {};
    storageAttachment4Desc.format = vulkanManager->swapChainImageFormat;
    storageAttachment4Desc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachment4Desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachment4Desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachment4Desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachment4Desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachment4Desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    storageAttachment4Desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachment4Ref = {};
    storageAttachment4Ref.attachment = 6;
    storageAttachment4Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription storageAttachment5Desc = {};
    storageAttachment5Desc.format = vulkanManager->swapChainImageFormat;
    storageAttachment5Desc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachment5Desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachment5Desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachment5Desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachment5Desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachment5Desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    storageAttachment5Desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachment5Ref = {};
    storageAttachment5Ref.attachment = 7;
    storageAttachment5Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription storageAttachment6Desc = {};
    storageAttachment6Desc.format = vulkanManager->swapChainImageFormat;
    storageAttachment6Desc.samples = VK_SAMPLE_COUNT_1_BIT;
    storageAttachment6Desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    storageAttachment6Desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    storageAttachment6Desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    storageAttachment6Desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    storageAttachment6Desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    storageAttachment6Desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference storageAttachment6Ref = {};
    storageAttachment6Ref.attachment = 8;
    storageAttachment6Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    std::vector<VkSubpassDescription> subpassArray{};
    subpassArray.resize(1 + 2 * (size_t) vulkanManager->graphicsOptions.gaussianBlurLevels);

    subpassArray[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassArray[0].colorAttachmentCount = 1;
    if (vulkanManager->graphicsOptions.samples != VK_SAMPLE_COUNT_1_BIT) {
        subpassArray[0].pColorAttachments = &multiAttachmentRef;
    } else {

        if ((int) vulkanManager->graphicsOptions.gaussianBlurLevels == 0){
            subpassArray[0].pColorAttachments = &swapChainAttachmentRef;
        } else {
            subpassArray[0].pColorAttachments = &storageAttachmentRef;
        }

    }
    subpassArray[0].pDepthStencilAttachment = &depthAttachmentRef;
    if (vulkanManager->graphicsOptions.samples != VK_SAMPLE_COUNT_1_BIT) {
        if ((int) vulkanManager->graphicsOptions.gaussianBlurLevels == 0){
            subpassArray[0].pResolveAttachments = &swapChainAttachmentRef;
        } else {
            subpassArray[0].pResolveAttachments = &storageAttachmentRef;
        }
    }

    //TODO fix?

    switch (vulkanManager->graphicsOptions.gaussianBlurLevels){
        case GaussianBlurLevels::DISABLED:break;
        case GaussianBlurLevels::ENABLED_1:

            subpassArray[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[1].colorAttachmentCount = 1;
            subpassArray[1].pColorAttachments = &storageAttachment2Ref;

            subpassArray[2].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[2].colorAttachmentCount = 1;
            subpassArray[2].pColorAttachments = &swapChainAttachmentRef;

            break;
        case GaussianBlurLevels::ENABLED_2:

            subpassArray[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[1].colorAttachmentCount = 1;
            subpassArray[1].pColorAttachments = &storageAttachment2Ref;

            subpassArray[2].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[2].colorAttachmentCount = 1;
            subpassArray[2].pColorAttachments = &storageAttachment3Ref;

            subpassArray[3].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[3].colorAttachmentCount = 1;
            subpassArray[3].pColorAttachments = &storageAttachment4Ref;

            subpassArray[4].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[4].colorAttachmentCount = 1;
            subpassArray[4].pColorAttachments = &swapChainAttachmentRef;

            break;
        case GaussianBlurLevels::ENABLED_3:

            subpassArray[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[1].colorAttachmentCount = 1;
            subpassArray[1].pColorAttachments = &storageAttachment2Ref;

            subpassArray[2].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[2].colorAttachmentCount = 1;
            subpassArray[2].pColorAttachments = &storageAttachment3Ref;

            subpassArray[3].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[3].colorAttachmentCount = 1;
            subpassArray[3].pColorAttachments = &storageAttachment4Ref;

            subpassArray[4].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[4].colorAttachmentCount = 1;
            subpassArray[4].pColorAttachments = &storageAttachment5Ref;

            subpassArray[5].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[5].colorAttachmentCount = 1;
            subpassArray[5].pColorAttachments = &storageAttachment6Ref;

            subpassArray[6].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassArray[6].colorAttachmentCount = 1;
            subpassArray[6].pColorAttachments = &swapChainAttachmentRef;

            break;
    }

    std::vector<VkSubpassDependency> dependencyArray{};

    dependencyArray.resize(1);
    dependencyArray[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencyArray[0].dstSubpass = 0;
    dependencyArray[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyArray[0].srcAccessMask = 0;
    dependencyArray[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyArray[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    dependencyArray.resize(1 + 2 * (size_t) vulkanManager->graphicsOptions.gaussianBlurLevels );

    for (int i = 0; i < (int) vulkanManager->graphicsOptions.gaussianBlurLevels; ++i) {

        //TODO check dependency is done right
        dependencyArray[1 + 2*i].srcSubpass = (uint32_t) 2*i;
        dependencyArray[1 + 2*i].dstSubpass = (uint32_t) 2*i+1;
        dependencyArray[1 + 2*i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencyArray[1 + 2*i].srcAccessMask = 0;
        dependencyArray[1 + 2*i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencyArray[1 + 2*i].dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;

        //TODO check dependency is done right
        dependencyArray[2 + 2*i].srcSubpass = (uint32_t) 2*i+1;
        dependencyArray[2 + 2*i].dstSubpass = (uint32_t) 2*i+2;
        dependencyArray[2 + 2*i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencyArray[2 + 2*i].srcAccessMask = 0;
        dependencyArray[2 + 2*i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencyArray[2 + 2*i].dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;

    }

    std::vector<VkAttachmentDescription> attachmentsDescs = { swapChainAttachmentDesc, depthAttachmentDesc, multiAttachmentDesc};

    switch (vulkanManager->graphicsOptions.gaussianBlurLevels){
        case GaussianBlurLevels::DISABLED:break;
        case GaussianBlurLevels::ENABLED_1:
            attachmentsDescs.push_back(storageAttachmentDesc);
            attachmentsDescs.push_back(storageAttachment2Desc);
            break;
        case GaussianBlurLevels::ENABLED_2:
            attachmentsDescs.push_back(storageAttachmentDesc);
            attachmentsDescs.push_back(storageAttachment2Desc);
            attachmentsDescs.push_back(storageAttachment3Desc);
            attachmentsDescs.push_back(storageAttachment4Desc);
            break;
        case GaussianBlurLevels::ENABLED_3:
            attachmentsDescs.push_back(storageAttachmentDesc);
            attachmentsDescs.push_back(storageAttachment2Desc);
            attachmentsDescs.push_back(storageAttachment3Desc);
            attachmentsDescs.push_back(storageAttachment4Desc);
            attachmentsDescs.push_back(storageAttachment5Desc);
            attachmentsDescs.push_back(storageAttachment6Desc);
            break;
    }

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t> (attachmentsDescs.size ());
    renderPassInfo.pAttachments = attachmentsDescs.data ();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subpassArray.size());
    renderPassInfo.pSubpasses = subpassArray.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencyArray.size());
    renderPassInfo.pDependencies = dependencyArray.data();

    DebugUtility::VkSuccess(vkCreateRenderPass (vulkanManager->device, &renderPassInfo, nullptr, &vulkanManager->renderPass), "Failed to create render pass!");

}

void RenderPassManager::cleanUp() {

    vkDestroyRenderPass(vulkanManager->device, vulkanManager->renderPass, nullptr);
    vulkanManager->renderPass = VK_NULL_HANDLE;

}
