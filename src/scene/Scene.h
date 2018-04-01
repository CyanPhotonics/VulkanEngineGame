#ifndef SCENE_H
#define SCENE_H

#include <set>

#include "../rendering/ImageManager.h"
#include "../resources/ModelManager.h"

struct TexturedModel {
    Model model{};
    Texture texture{};
};

struct Entity {
    glm::vec3 position{};
    glm::vec3 scale{};
    glm::vec3 rotation{};

    TexturedModel texturedModel{};
};

typedef int Entity_T;

static const size_t MAX_POINT_LIGHTS = 100;

struct PointLight {

    Entity_T entity = -1;

    glm::vec3 attenuation{};
    glm::vec3 colour{};
    glm::vec3 position{};

    bool hasTexturedModel = true;
};

typedef struct{
    Entity_T entity;
    int pointLight;
} PointLight_T;

struct Scene {
    std::vector<Entity> entities = {};
    std::vector<PointLight> pointLights = {};

    std::vector<Texture> storageTextures = {};
};

#endif //SCENE_H
