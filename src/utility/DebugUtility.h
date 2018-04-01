#ifndef DEBUG_UTILITY_H
#define DEBUG_UTILITY_H

#include <iostream>

#include <glm/glm.hpp>

class DebugUtility{
public:

    static void print(const char* name, glm::mat4 &mat4){
        std::cout << "----" << name << "----" << std::endl;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::cout << mat4[i][j] << (j != 3 ? " " : "");
            }
            std::cout << std::endl;
        }
    }

};

#endif //DEBUG_UTILITY_H
