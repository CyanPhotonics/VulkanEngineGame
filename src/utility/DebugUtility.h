#ifndef DEBUG_UTILITY_H
#define DEBUG_UTILITY_H

#include <vulkan/vulkan.h>
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

    static void VkSuccess(VkResult result, std::string message){

        if (result != VK_SUCCESS){
            std::stringstream stream;

            stream
                << message
                << "\n"
                << "Error: \n"
                << result
                << "\n";

            throw std::runtime_error(stream.str());

        }
    }

};

#endif //DEBUG_UTILITY_H
