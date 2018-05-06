#ifndef DEBUG_UTILITY_H
#define DEBUG_UTILITY_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <sstream>

#include <glm/glm.hpp>

class DebugUtility{
public:

    //TODO disable when not debug?

    static void print(const char* name, glm::mat4 &mat4){
        std::cout << "----" << name << "----" << std::endl;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::cout << mat4[i][j] << (j != 3 ? " " : "");
            }
            std::cout << std::endl;
        }
    }

    static void ifSet(VkResult result, VkResult desiredResult, std::string name, std::string& storageString){
        if (result == desiredResult){
            storageString = name;
        }
    }

    static void VkSuccess(VkResult result, std::string message){

        if (result != VK_SUCCESS){
            std::stringstream stream;

            std::string errorName;

            ifSet(result, VK_NOT_READY, "VK_NOT_READY", errorName);
            ifSet(result, VK_TIMEOUT, "VK_TIMEOUT", errorName);
            ifSet(result, VK_EVENT_SET, "VK_EVENT_SET", errorName);
            ifSet(result, VK_EVENT_RESET, "VK_EVENT_RESET", errorName);
            ifSet(result, VK_INCOMPLETE, "VK_INCOMPLETE", errorName);
            ifSet(result, VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY", errorName);
            ifSet(result, VK_ERROR_OUT_OF_DEVICE_MEMORY, "VK_ERROR_OUT_OF_DEVICE_MEMORY", errorName);
            ifSet(result, VK_ERROR_INITIALIZATION_FAILED, "VK_ERROR_INITIALIZATION_FAILED", errorName);
            ifSet(result, VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST", errorName);
            ifSet(result, VK_ERROR_MEMORY_MAP_FAILED, "VK_ERROR_MEMORY_MAP_FAILED", errorName);
            ifSet(result, VK_ERROR_LAYER_NOT_PRESENT, "VK_ERROR_LAYER_NOT_PRESENT", errorName);
            ifSet(result, VK_ERROR_EXTENSION_NOT_PRESENT, "VK_ERROR_EXTENSION_NOT_PRESENT", errorName);
            ifSet(result, VK_ERROR_FEATURE_NOT_PRESENT, "VK_ERROR_FEATURE_NOT_PRESENT", errorName);
            ifSet(result, VK_ERROR_INCOMPATIBLE_DRIVER, "VK_ERROR_INCOMPATIBLE_DRIVER", errorName);
            ifSet(result, VK_ERROR_TOO_MANY_OBJECTS, "VK_ERROR_TOO_MANY_OBJECTS", errorName);
            ifSet(result, VK_ERROR_FORMAT_NOT_SUPPORTED, "VK_ERROR_FORMAT_NOT_SUPPORTED", errorName);
            ifSet(result, VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL", errorName);
            ifSet(result, VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY", errorName);
            ifSet(result, VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE", errorName);
            ifSet(result, VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR", errorName);
            ifSet(result, VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR", errorName);
            ifSet(result, VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR", errorName);
            ifSet(result, VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR", errorName);
            ifSet(result, VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR", errorName);
            ifSet(result, VK_ERROR_VALIDATION_FAILED_EXT, "VK_ERROR_VALIDATION_FAILED_EXT", errorName);
            ifSet(result, VK_ERROR_INVALID_SHADER_NV, "VK_ERROR_INVALID_SHADER_NV", errorName);
            ifSet(result, VK_ERROR_NOT_PERMITTED_EXT, "VK_ERROR_NOT_PERMITTED_EXT", errorName);

            stream
                << message << "\n"
                << "Error Code: " << result << "\n"
                << "Error Name: " << errorName << "\n";

            throw std::runtime_error(stream.str());

        }
    }

};

#endif //DEBUG_UTILITY_H
