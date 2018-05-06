#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Scene.h"

class SceneManager {
public:
    ImageManager* imageManager;
    ModelManager* modelManager;

    SceneManager(ImageManager* imageManager, ModelManager* modelManager) : imageManager(imageManager), modelManager(modelManager) {}

    Entity_T addEntity(Scene &scene, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, const char *modelFile,
                       const char *textureFile);
    PointLight_T addPointLight(Scene &scene, glm::vec3 attenuation, glm::vec3 colour, glm::vec3 pos, glm::vec3 rot,
                               glm::vec3 scale, const char *modelFile, const char *textureFile);
    PointLight_T addPointLight(Scene &scene, glm::vec3 attenuation, glm::vec3 colour, glm::vec3 pos);

    void cleanUp(Scene &scene);
};


#endif //SCENE_MANAGER_H
