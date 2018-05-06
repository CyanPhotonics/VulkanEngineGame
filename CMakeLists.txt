cmake_minimum_required(VERSION 3.5.2)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

if ($ENV{WSL})
    set(LIB_DIR "/mnt/c/Users/Matthew/CLionProjects/Libraries")
    set(VULKAN_DIR "/home/Matthew/VulkanSDK/1.1.70.1/x86_64/")
else ($ENV{WSL})
    set(LIB_DIR "C:/Users/Matthew/CLionProjects/Libraries")
    set(VULKAN_DIR "C:/VulkanSDK/1.1.73.0/")
endif ($ENV{WSL})

include_directories(
        if ($ENV{WSL})
            ${VULKAN_DIR}/include
        else ($ENV{WSL})
            ${VULKAN_DIR}/Include
        endif ($ENV{WSL})

        ${LIB_DIR}/glm
        ${LIB_DIR}/glfw/include
        ${LIB_DIR}/stb
        ${LIB_DIR}/tinyobjloader
        # ${LIB_DIR}/OBJ_Loader
)

link_directories(
        if ($ENV{WSL})
            "/usr/local/lib"
            "/usr/lib/x86_64-linux-gnu"
            ${VULKAN_DIR}/lib
        else ($ENV{WSL})
            ${VULKAN_DIR}/Lib
            ${LIB_DIR}/glfw/lib-mingw-w64
        endif ($ENV{WSL})
)

add_executable(VulkanEngine

        src/testing/EngineTester.cpp src/testing/EngineTester.h

        src/testing/Camera.cpp src/testing/Camera.h

        src/scene/SceneManager.cpp src/scene/SceneManager.h
        src/scene/Scene.h

        src/vulkanSetup/VulkanManager.h
        src/vulkanSetup/InstanceManager.cpp         src/vulkanSetup/InstanceManager.h
        src/vulkanSetup/ValidationLayerManager.cpp  src/vulkanSetup/ValidationLayerManager.h
        src/vulkanSetup/DeviceManager.cpp           src/vulkanSetup/DeviceManager.h

        src/presentation/SwapChainManager.cpp   src/presentation/SwapChainManager.h
        src/presentation/WindowManager.cpp      src/presentation/WindowManager.h

        src/rendering/RenderPassManager.cpp             src/rendering/RenderPassManager.h
        src/rendering/FrameBufferManager.cpp            src/rendering/FrameBufferManager.h
        src/rendering/ImageManager.cpp                  src/rendering/ImageManager.h
        src/rendering/CommandPoolManager.cpp            src/rendering/CommandPoolManager.h
        src/rendering/BasicCommandBufferManager.cpp     src/rendering/BasicCommandBufferManager.h

        src/graphicsPipeline/GraphicsPipelineManager.cpp    src/graphicsPipeline/GraphicsPipelineManager.h
        src/graphicsPipeline/BasicPipelines.cpp             src/graphicsPipeline/BasicPipelines.h
        src/graphicsPipeline/UniformVariable.cpp            src/graphicsPipeline/UniformVariable.h
        src/graphicsPipeline/TextureSamplers.h

        src/postProcessing/PostProcessingStage.cpp      src/postProcessing/PostProcessingStage.h
        src/postProcessing/GaussianBlur.cpp             src/postProcessing/GaussianBlur.h

        src/resources/Texture.h
        src/resources/Model.h
        src/resources/ModelManager.cpp  src/resources/ModelManager.h
        src/resources/OBJ_Loader.h

        src/utility/CollectionUtility.h
        src/utility/Maths.h
        src/utility/MemoryUtility.h     src/utility/MemoryUtility.cpp
        src/utility/DebugUtility.h

        src/rendering/GraphicsOptions.h)

target_link_libraries(VulkanEngine
        ${GLFW3_LIBRARY}
        ${VULKAN-1_LIBRARY}
)

if ($ENV{WSL})
    target_link_libraries(VulkanEngine
        glfw
        "/home/Matthew/VulkanSDK/1.1.70.1/x86_64/lib/libvulkan.so"
    )
else ($ENV{WSL})
    target_link_libraries(VulkanEngine
        glfw3
        vulkan-1
    )
endif ($ENV{WSL})

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
