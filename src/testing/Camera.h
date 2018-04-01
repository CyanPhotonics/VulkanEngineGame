#ifndef CAMERA_H
#define CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../presentation/WindowManager.h"

#include "../utility/Maths.h"

class Camera {
private:
    glm::vec3 position;
    glm::vec3 lookNormal;
    glm::mat4 viewMatrix;

    float pitch = 0;
    float yaw = 0;

    const float speed1 = 1.0;
    const float speed2 = 3.0;

    float speed = 1.0;

    const float mouseSensitivity = 0.5;

    bool infoChanged = false;

    const glm::vec3 up = {0,0,1};

    void generateNormal();

public:
    Camera(glm::vec3 position, glm::vec3 lookNormal){
        this->position = position;
        this->lookNormal = lookNormal;
    }

    void move(WindowManager &windowManager, GLFWwindow* window);
    glm::mat4 getViewMatrix();

    float FOV = 110.0;
    const float zNear = 0.01;
    //const float zFar = std::numeric_limits<float>::infinity();
    const float zFar = 1000.0;
};


#endif //CAMERA_H
