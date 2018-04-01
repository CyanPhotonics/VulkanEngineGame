#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
//#include <tiny_obj_loader.h>

#include "Model.h"

#include "../vulkanSetup/VulkanManager.h"
#include "../utility/MemoryUtility.h"

class ModelManager {
private:
    VulkanManager* vulkanManager{};
    MemoryUtility* memoryUtility{};

    std::unordered_map<const char*, Model> models = {};

    const char* modelFolder = "res/models/";

    void createVertexBuffer(Model &model);
    void createIndexBuffer(Model &model);
public:
    ModelManager(VulkanManager *vulkanManager, MemoryUtility *memoryUtility){
        this->vulkanManager = vulkanManager;
        this->memoryUtility = memoryUtility;
    }

    Model loadIntoLocal(const char *file);
    void loadIntoDevice(Model &model);
    void unloadFromDevice(Model &model);
    void unloadFromLocal(Model &model);
    void cleanUpModels();

};

#endif //MODEL_MANAGER_H
