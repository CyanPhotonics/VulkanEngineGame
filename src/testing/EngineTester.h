#ifndef TESTER_H
#define TESTER_H

#include <vector>
#include <chrono>

#include "../vulkanSetup/InstanceManager.h"
#include "../vulkanSetup/VulkanManager.h"
#include "../vulkanSetup/ValidationLayerManager.h"
#include "../vulkanSetup/DeviceManager.h"

#include "../presentation/WindowManager.h"
#include "../presentation/SwapChainManager.h"

#include "../rendering/CommandPoolManager.h"
#include "../rendering/ImageManager.h"
#include "../rendering/GraphicsPipelineManager.h"
#include "../rendering/FrameBufferManager.h"
#include "../rendering/RenderPassManager.h"
#include "../rendering/BasicCommandBufferManager.h"

#include "../graphicsPipeline/BasicPipelines.h"

#include "../resources/ModelManager.h"

#include "../scene/SceneManager.h"

#include "../utility/CollectionUtility.h"
#include "../utility/MemoryUtility.h"
#include "../utility/DebugUtility.h"

#include "Camera.h"

class EngineTester {
private:
    std::vector<const char*> extensions = {};
    #ifndef NDEBUG
        std::vector<const char*> validationLayers = {
                "VK_LAYER_LUNARG_standard_validation",
                //"VK_LAYER_LUNARG_assistant_layer" //TODO fix?
        };
    #else
        std::vector<const char*> validationLayers = {};
    #endif

    VulkanManager vulkanManager = VulkanManager();

    ValidationLayerManager validationLayerManager = ValidationLayerManager(&vulkanManager);
    InstanceManager instanceManager = InstanceManager(&vulkanManager);
    WindowManager windowManager = WindowManager(&vulkanManager);
    DeviceManager deviceManager = DeviceManager(&vulkanManager);
    SwapChainManager swapChainManager = SwapChainManager(&vulkanManager);

    CommandPoolManager commandPoolManager = CommandPoolManager(&vulkanManager);
    MemoryUtility memoryUtility = MemoryUtility(&vulkanManager, &commandPoolManager);
    ImageManager imageManager = ImageManager(&vulkanManager, &commandPoolManager, &memoryUtility);
    RenderPassManager renderPassManager = RenderPassManager(&vulkanManager);
    FrameBufferManager frameBufferManager = FrameBufferManager(&vulkanManager);
    BasicCommandBufferManager basicCommandBufferManager = BasicCommandBufferManager(&vulkanManager);

    GraphicsPipelineManager graphicsPipelineManager = GraphicsPipelineManager(&vulkanManager, &memoryUtility);
    BasicPipelines basicPipelines = BasicPipelines();

    ModelManager modelManager = ModelManager(&vulkanManager, &memoryUtility);

    Camera camera = Camera({-2,0,0}, {1,0,0});
    SceneManager sceneManager = SceneManager(&imageManager, &modelManager);

    Scene scene{};

    Entity_T crate{};
    PointLight_T light{};
    PointLight_T light2{};

    uint32_t imageIndex = std::numeric_limits<uint32_t>::max();

    void createVulkanEnvironment();
    void createScene();
    void postSwapChain();
    void preloop();
    void updateUniforms(int start, int count, int pipelineIndex);
    void loopLogic();
    void loopRender();
    void postLoop();
    void cleanup();
public:
    void run();

    void cleanUpSwapChain();
    void createSwapChain();

    void updateWindowSize(int width, int height){ windowManager.updateWindowSize(width, height); }

};

#endif //ENGINE_TESTER_H