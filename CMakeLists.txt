cmake_minimum_required(VERSION 3.9)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

set(LIB_DIR "C:/Users/Matthew/CLionProjects/Libraries")
set(VULKAN_DIR "C:/VulkanSDK/1.1.70.0/")

include_directories(
        ${LIB_DIR}/glm
        ${LIB_DIR}/glfw/include
        ${VULKAN_DIR}/Include
        ${LIB_DIR}/stb
        ${LIB_DIR}/tinyobjloader
        # ${LIB_DIR}/OBJ_Loader
)

link_directories(
        ${LIB_DIR}/glfw/lib-mingw-w64
        ${VULKAN_DIR}/Lib
)

add_executable(VulkanEngine

        src/testing/EngineTester.cpp src/testing/EngineTester.h

        src/testing/Camera.cpp src/testing/Camera.h

        src/scene/SceneManager.cpp src/scene/SceneManager.h
        src/scene/Scene.h

        src/vulkanSetup/VulkanManager.h
        src/vulkanSetup/InstanceManager.cpp src/vulkanSetup/InstanceManager.h
        src/vulkanSetup/ValidationLayerManager.cpp src/vulkanSetup/ValidationLayerManager.h
        src/vulkanSetup/DeviceManager.cpp src/vulkanSetup/DeviceManager.h

        src/presentation/SwapChainManager.cpp src/presentation/SwapChainManager.h
        src/presentation/WindowManager.cpp src/presentation/WindowManager.h

        src/rendering/RenderPassManager.cpp src/rendering/RenderPassManager.h
        src/rendering/GraphicsPipelineManager.cpp src/rendering/GraphicsPipelineManager.h
        src/rendering/FrameBufferManager.cpp src/rendering/FrameBufferManager.h
        src/rendering/ImageManager.cpp src/rendering/ImageManager.h
        src/rendering/CommandPoolManager.cpp src/rendering/CommandPoolManager.h
        src/rendering/BasicCommandBufferManager.cpp src/rendering/BasicCommandBufferManager.h

        src/graphicsPipeline/BasicPipelines.cpp src/graphicsPipeline/BasicPipelines.h

        src/resources/Texture.h
        src/resources/Model.h
        src/resources/ModelManager.cpp src/resources/ModelManager.h
        src/resources/OBJ_Loader.h

        src/utility/CollectionUtility.h
        src/utility/Maths.h
        src/utility/MemoryUtility.h src/utility/MemoryUtility.cpp
        src/utility/DebugUtility.h)

target_link_libraries(VulkanEngine
        ${GLFW3_LIBRARY}
        ${VULKAN-1_LIBRARY}
)

target_link_libraries(VulkanEngine
        glfw3
        vulkan-1
)

#add_custom_command(
#        TARGET VulkanEngine PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E remove_directory
#        $<TARGET_FILE_DIR:VulkanEngine>/res
#)
#
#add_custom_command(
#        TARGET VulkanEngine POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:VulkanEngine>/res
#)
