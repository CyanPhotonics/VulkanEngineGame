#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "EngineTester.h"

void EngineTester::createVulkanEnvironment() {

    windowManager.initWindow(const_cast<char *>("Vulkan Engine"), 1280, 720, this, false);

    validationLayerManager.validateLayerSupport(validationLayers, extensions);
    instanceManager.createInstance("Vulkan Engine",
                                   CollectionUtility::combine(windowManager.getRequiredExtensions(), extensions),
                                   validationLayers);
    validationLayerManager.setup(validationLayers);

    windowManager.createSurface();

    deviceManager.pickPhysicalDevice();
    deviceManager.createLogicalDevice(validationLayers);

    commandPoolManager.createCommandPools();

}

void EngineTester::createScene() {

    sceneManager.addEntity(scene,
                           glm::vec3(0), glm::vec3(M_PI_2, 0, 0), glm::vec3(100),
                           "plane.obj", "white.png"
    );

    sceneManager.addEntity(scene,
                           glm::vec3(5, 5, 1.5), glm::vec3(0), glm::vec3(3),
                           "cube.obj", "blue.png"
    );

    sceneManager.addPointLight(scene,
                               glm::vec3(0.01, 0, 1), glm::vec3(1),
                               glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0.25f),
                               "sphere.obj", "white.png"
    );

    for (int i = 0; i < 0; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                sceneManager.addEntity(scene,
                                       glm::vec3(7 + (2 * i), 7 + (2 * j), 3.5 + (2 * k)), glm::vec3(0), glm::vec3(1),
                                       "sphere.obj", "blue.png"
                );
            }
        }
    }

    {

        vulkanManager.graphicsOptions.samples = VK_SAMPLE_COUNT_8_BIT;
        vulkanManager.graphicsOptions.gaussianBlurLevels = GaussianBlurLevels::DISABLED;
        vulkanManager.graphicsOptions.gaussianBlurFactor = GaussianBlurFactor::DISABLED;

    }

    basicPipelines.createPipelines(&vulkanManager);
    graphicsPipelineManager.initialSetup(basicPipelines.pipelines, &scene);

}

std::vector<GaussianBlur *> gaussianBlurs{};

std::vector<PostProcessingStage> postProcessStages{};

void EngineTester::createSwapChain(VkSwapchainKHR oldSwapChain) {

    swapChainManager.createSwapChainImagesAndViews(windowManager, oldSwapChain);

    if (vulkanManager.swapChainExtent.width == 0 || vulkanManager.swapChainExtent.height == 0) {
        cleanUpSwapChain();
        return;
    };

    imageManager.createDepthResources();
    imageManager.createMSAAResource();

    for (int i = 0; i < (int) vulkanManager.graphicsOptions.gaussianBlurLevels; ++i) {
        imageManager.createStorageImageResource(scene.storageTextures, 1.0f);
        imageManager.createStorageImageResource(scene.storageTextures, 1.0f);
    }

    renderPassManager.createRenderPassesForSwapChain();

    std::vector<VkImageView> extraImageView{};
    extraImageView.push_back(imageManager.MSAATexture.imageView);

    for (int i = 0; i < (int) vulkanManager.graphicsOptions.gaussianBlurLevels; ++i) {
        extraImageView.push_back(scene.storageTextures[2*i].imageView);
        extraImageView.push_back(scene.storageTextures[1+2*i].imageView);
    }

    frameBufferManager.createFrameBuffersWithSwapChain(extraImageView);

    gaussianBlurs.clear();

    float factor = 1.0f / (int) vulkanManager.graphicsOptions.gaussianBlurFactor;
    for (int i = 0; i < (int) vulkanManager.graphicsOptions.gaussianBlurLevels; ++i) {
        gaussianBlurs.push_back(new GaussianBlur{(uint32_t ) 1 + 2 * i,scene.storageTextures[0+2*i],scene.storageTextures[1+2*i], (float) std::pow(factor, (i+1)) ,&graphicsPipelineManager,&vulkanManager});
    }

    postProcessStages.clear();

    for (GaussianBlur* gaussianBlur : gaussianBlurs){
        postProcessStages.push_back(gaussianBlur->getHorizontalStage());
        postProcessStages.push_back(gaussianBlur->getVerticalStage());
    }

    for (int i = 0; i < (int) vulkanManager.graphicsOptions.gaussianBlurLevels; ++i) { //TODO Research input attachments for potential solution
        graphicsPipelineManager.textureSamplers[1+2*i][0].accessImageInfo(0).imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //TODO fix?       //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        graphicsPipelineManager.textureSamplers[2+2*i][0].accessImageInfo(0).imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//TODO fix?       //VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    }

    graphicsPipelineManager.perSwapChainSetup();

    basicCommandBufferManager.createCommandBuffers(frameBufferManager.getFrameBuffers(), graphicsPipelineManager, scene,
                                                   windowManager, postProcessStages);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vulkanManager.imageAvailableSemaphores.resize(vulkanManager.swapChainImageViews.size());
    for (int i = 0; i < vulkanManager.swapChainImageViews.size(); ++i) {
        if (vkCreateSemaphore(vulkanManager.device, &semaphoreInfo, nullptr,
                              &vulkanManager.imageAvailableSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphores");
        }
    }

    imageIndex = std::numeric_limits<uint32_t>::max();

}

void EngineTester::postSwapChain() {

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(vulkanManager.device, &semaphoreInfo, nullptr, &vulkanManager.renderFinishedSemaphore) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphores");
    }

}

void EngineTester::preloop() {

}

long long int frame = 0;

void EngineTester::loopLogic() {

    frame++;

    //Move stuff

    camera.move(windowManager, vulkanManager.window);


    //Update uniform buffer


    glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.FOV), vulkanManager.swapChainExtent.width /
                                                                            (float) vulkanManager.swapChainExtent.height,
                                                  camera.zNear, camera.zFar);
    projectionMatrix[1][1] *= -1;
    glm::mat4 viewMatrix = camera.getViewMatrix();

    glm::mat4 projectionViewMatrix = glm::mat4(projectionMatrix) * viewMatrix;

    void *data;

    vkQueueWaitIdle(vulkanManager.graphicsQueue);

    //Load static

    //Camera

    //                                                                        pipeline,uniform,array index
    vkMapMemory(vulkanManager.device, graphicsPipelineManager.staticUniformVariables[0][0].accessMemory(), 0,
                sizeof(projectionViewMatrix), 0, &data);
    memcpy(data, &projectionViewMatrix, sizeof(projectionViewMatrix));
    vkUnmapMemory(vulkanManager.device, graphicsPipelineManager.staticUniformVariables[0][0].accessMemory());

    basicPipelines.blurData.imageWidth = vulkanManager.swapChainExtent.width;
    basicPipelines.blurData.imageHeight = vulkanManager.swapChainExtent.height;

    for (int i = 0; i < gaussianBlurs.size(); ++i) {

        basicPipelines.blurData.resScale = gaussianBlurs[i]->getResolutionScale();

        vkMapMemory (vulkanManager.device, graphicsPipelineManager.staticUniformVariables[gaussianBlurs[i]->getHorizontalStage().getSubpassIndex()][0].accessMemory(), 0, sizeof (basicPipelines.blurData), 0, &data);
        memcpy (data, &basicPipelines.blurData, sizeof (basicPipelines.blurData));
        vkUnmapMemory (vulkanManager.device, graphicsPipelineManager.staticUniformVariables[gaussianBlurs[i]->getHorizontalStage().getSubpassIndex()][0].accessMemory());

        vkMapMemory (vulkanManager.device, graphicsPipelineManager.staticUniformVariables[gaussianBlurs[i]->getVerticalStage().getSubpassIndex()][0].accessMemory(), 0, sizeof (basicPipelines.blurData), 0, &data);
        memcpy (data, &basicPipelines.blurData, sizeof (basicPipelines.blurData));
        vkUnmapMemory (vulkanManager.device, graphicsPipelineManager.staticUniformVariables[gaussianBlurs[i]->getVerticalStage().getSubpassIndex()][0].accessMemory());

    }

    //Lights

    //Point lights

    if (frame == 9) {


        uint32_t pointLightCount = (uint32_t) std::min(MAX_POINT_LIGHTS, scene.pointLights.size());

        basicPipelines.pointLightUO.count = pointLightCount;

        for (int i = 0; i < pointLightCount; ++i) {

            if (scene.pointLights[i].entity != -1) {
                basicPipelines.pointLightUO.positions[i] = glm::vec4(
                        scene.entities[scene.pointLights[i].entity].position, 0);
            } else {
                basicPipelines.pointLightUO.positions[i] = glm::vec4(scene.pointLights[i].position, 0);
            }
            basicPipelines.pointLightUO.attenuations[i] = glm::vec4(scene.pointLights[i].attenuation, 0);
            basicPipelines.pointLightUO.colours[i] = glm::vec4(scene.pointLights[i].colour, 0);

        }

        vkMapMemory(vulkanManager.device, graphicsPipelineManager.staticUniformVariables[0][1].accessMemory(), 0,
                    sizeof(BasicPipelines::PointLightUniformObject), 0, &data);
        memcpy(data, &basicPipelines.pointLightUO, sizeof(BasicPipelines::PointLightUniformObject));
        vkUnmapMemory(vulkanManager.device, graphicsPipelineManager.staticUniformVariables[0][1].accessMemory());

    }

    //Non static

    if (frame == 10) {

        int threadCount = 8;
        std::thread threads[threadCount];

        uint32_t used = 0;
        auto left = static_cast<uint32_t >(scene.entities.size());
        auto minStep = static_cast<uint32_t >(std::floor(left / threadCount));

        for (uint32_t i = 0; i < threadCount; ++i) {
            if (i != threadCount - 1) {
                threads[i] = std::thread(&EngineTester::updateUniforms, this, used, minStep, 0);
                used += minStep;
                left -= minStep;
            } else {
                threads[i] = std::thread(&EngineTester::updateUniforms, this, used, left, 0);
            }
        }

        for (int i = 0; i < threadCount; ++i) {
            threads[i].join();
        }

    }

}

void EngineTester::updateUniforms(uint32_t start, uint32_t count, uint32_t pipelineIndex) {

    void *data;

    glm::mat4 modelMatrix{};
    for (uint32_t i = start; i < start + count; ++i) {

        Entity &entity = scene.entities[i];

        modelMatrix = glm::translate(entity.position)
                      * glm::rotate(entity.rotation.x, glm::vec3(1, 0, 0))
                      * glm::rotate(entity.rotation.y, glm::vec3(0, 1, 0))
                      * glm::rotate(entity.rotation.z, glm::vec3(0, 0, 1))
                      * glm::scale(entity.scale);

//        DebugUtility::print("Model Matrix", modelMatrix);

        //                                                                          pipeline, uniform
        vkMapMemory(vulkanManager.device, graphicsPipelineManager.uniformVariables[pipelineIndex][0].accessMemory(i), 0,
                    sizeof(modelMatrix), 0,
                    &data);
        memcpy(data, &modelMatrix, sizeof(modelMatrix));
        vkUnmapMemory(vulkanManager.device, graphicsPipelineManager.uniformVariables[pipelineIndex][0].accessMemory(i));


    }

}

void EngineTester::loopRender() {

    static VkResult result = VK_SUCCESS;

    if (windowManager.shouldNotRender()) return;;

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){
        result = VK_SUCCESS;
        vulkanManager.oldSwapChain = vulkanManager.swapChain;
        cleanUpSwapChain();
        createSwapChain(vulkanManager.oldSwapChain);
        swapChainManager.cleanUp(vulkanManager.oldSwapChain);
    }

    if (imageIndex == std::numeric_limits<uint32_t>::max()) {
        imageIndex = static_cast<uint32_t>(vulkanManager.imageAvailableSemaphores.size() - 1);
    }

    //Draw frame
    vkQueueWaitIdle(vulkanManager.presentQueue);

    result = vkAcquireNextImageKHR(vulkanManager.device, vulkanManager.swapChain,
                                   std::numeric_limits<uint64_t>::max(),
                                   vulkanManager.imageAvailableSemaphores[(imageIndex + 1) %
                                                                                       vulkanManager.imageAvailableSemaphores.size()],
                                   VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        if (!windowManager.shouldNotRender()) {
            vulkanManager.oldSwapChain = vulkanManager.swapChain;
            cleanUpSwapChain();
            createSwapChain(vulkanManager.oldSwapChain);
            swapChainManager.cleanUp(vulkanManager.oldSwapChain);
            printf("Out of data\n");
            return;
        }
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!\n");
    }
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {vulkanManager.imageAvailableSemaphores[imageIndex]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &basicCommandBufferManager.commandBuffers[imageIndex];
    //printf("Image Index: %d\n", imageIndex);

    VkSemaphore signalSemaphores[] = {vulkanManager.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    DebugUtility::VkSuccess(vkQueueSubmit(vulkanManager.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE),
                            "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vulkanManager.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(vulkanManager.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        if (!windowManager.shouldNotRender()) {
            vulkanManager.oldSwapChain = vulkanManager.swapChain;
            cleanUpSwapChain();
            createSwapChain(vulkanManager.oldSwapChain);
            swapChainManager.cleanUp(vulkanManager.oldSwapChain);
            printf("Out of date/suboptimal\n");
        }
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!\n");
    }

}

void EngineTester::postLoop() {

}

void EngineTester::cleanUpSwapChain() {

    vkDeviceWaitIdle(vulkanManager.device);

    for (GaussianBlur *gaussianBlur : gaussianBlurs) {
        delete gaussianBlur;
    }

    basicCommandBufferManager.cleanUp();
    graphicsPipelineManager.swapChainCleanUp();
    imageManager.cleanUpDepthImage();
    imageManager.cleanUpExtraImages(scene.storageTextures);
    frameBufferManager.cleanUp();
    renderPassManager.cleanUp();
    swapChainManager.cleanUpImageViews();

    for (VkSemaphore &semaphore : vulkanManager.imageAvailableSemaphores) {
        vkDestroySemaphore(vulkanManager.device, semaphore, nullptr);
        semaphore = VK_NULL_HANDLE;
    }

}

void EngineTester::cleanup() {

    swapChainManager.cleanUp();

    sceneManager.cleanUp(scene);

    vkDestroySemaphore(vulkanManager.device, vulkanManager.renderFinishedSemaphore, nullptr);

    modelManager.cleanUpModels();

    imageManager.cleanUpTextures();

    graphicsPipelineManager.finalCleanUp();

    commandPoolManager.cleanUp();

    deviceManager.cleanUp();

    windowManager.cleanUp();

    if (!validationLayers.empty()) {
        validationLayerManager.cleanUp();
    }

    instanceManager.cleanup();

}

void EngineTester::run() {
    createVulkanEnvironment();
    printf("Vulkan environment setup complete\n");
    createScene();
    printf("Scene created\n");
    createSwapChain(nullptr);
    printf("Swap chain complete\n");
    postSwapChain();
    preloop();
    printf("Pre loop complete\n");

    windowManager.showWindow();
    while (windowManager.isOpen()) {
        windowManager.update();

        if (vulkanManager.swapChainExtent.width == 0 || vulkanManager.swapChainExtent.height == 0) continue;

        loopLogic();

        vkQueueWaitIdle(vulkanManager.presentQueue);

        loopRender();
    }
    postLoop();

    vkDeviceWaitIdle(vulkanManager.device);

    cleanUpSwapChain();
    cleanup();
}

int main() {

    EngineTester engineTester = EngineTester();

    try {
        engineTester.run();
    } catch (std::exception& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}