#include "SceneManager.h"
#include "Scene.h"

Entity_T SceneManager::addEntity(Scene &scene, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, const char *modelFile,
                                 const char *textureFile) {

    TexturedModel texturedModel{};

    texturedModel.model = modelManager->loadIntoLocal(modelFile);
    modelManager->loadIntoDevice(texturedModel.model);
    modelManager->unloadFromLocal(texturedModel.model);

    texturedModel.texture = imageManager->loadIntoLocal(textureFile);
    imageManager->loadIntoDevice(texturedModel.texture);
    imageManager->unloadFromLocal(texturedModel.texture);

    scene.entities.resize(scene.entities.size() + 1);

    scene.entities[scene.entities.size()-1].texturedModel = texturedModel;
    scene.entities[scene.entities.size()-1].position = pos;
    scene.entities[scene.entities.size()-1].rotation = rot;
    scene.entities[scene.entities.size()-1].scale = scale;

    return static_cast<Entity_T>(scene.entities.size() - 1);

}

PointLight_T SceneManager::addPointLight(Scene &scene, glm::vec3 attenuation, glm::vec3 colour, glm::vec3 pos) {
    return addPointLight(scene, attenuation, colour, pos, glm::vec3(0), glm::vec3(1), nullptr, nullptr);
}

PointLight_T SceneManager::addPointLight(Scene &scene, glm::vec3 attenuation, glm::vec3 colour, glm::vec3 pos,
                                         glm::vec3 rot,
                                         glm::vec3 scale, const char *modelFile, const char *textureFile) {

    scene.pointLights.resize(scene.pointLights.size() + 1);

    scene.pointLights[scene.pointLights.size()-1].attenuation = attenuation;
    scene.pointLights[scene.pointLights.size()-1].colour = colour;
    scene.pointLights[scene.pointLights.size()-1].position = pos;

    if (modelFile != nullptr){

        scene.pointLights[scene.pointLights.size()-1].entity = addEntity(scene, pos, rot, scale, modelFile, textureFile);
        scene.pointLights[scene.pointLights.size()-1].hasTexturedModel = true;

    }

    PointLight_T pointLight_t{};
    pointLight_t.entity = scene.pointLights[scene.pointLights.size()-1].entity;
    pointLight_t.pointLight = static_cast<int>(scene.pointLights.size() - 1);

    return pointLight_t;

}

void SceneManager::cleanUp(Scene &scene) {

    for (auto &entity : scene.entities) {
        modelManager->unloadFromDevice(entity.texturedModel.model);
        imageManager->unloadFromDevice(entity.texturedModel.texture);
    }

//    for (auto &pointLight : scene.pointLights) {
//        if (!pointLight.hasTexturedModel) continue;
//        modelManager->unloadFromDevice(pointLight.entitytexturedModel.model);
//        modelManager->unloadFromLocal(pointLight.texturedModel.model);
//        imageManager->unloadFromDevice(pointLight.texturedModel.texture);
//        imageManager->unloadFromLocal(pointLight.texturedModel.texture);
//    }

}