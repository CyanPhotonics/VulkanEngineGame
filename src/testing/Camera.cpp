#include "Camera.h"

void Camera::move(WindowManager &windowManager, GLFWwindow* window) {

    if (windowManager.isKeyDown(GLFW_KEY_LEFT_SHIFT)){
        speed = static_cast<float>(speed2 * windowManager.getLastFrameTime());
    } else {
        speed = static_cast<float>(speed1 * windowManager.getLastFrameTime());
    }

    //Forward, Sideways
    glm::vec2 movement = {0,0};

    if (windowManager.isKeyDown(GLFW_KEY_W)){
        movement.x += 1;
    }
    if (windowManager.isKeyDown(GLFW_KEY_S)){
        movement.x -= 1;
    }

    if (windowManager.isKeyDown(GLFW_KEY_D)){
        movement.y -= 1;
    }
    if (windowManager.isKeyDown(GLFW_KEY_A)){
        movement.y += 1;
    }

    if (windowManager.isKeyDown(GLFW_KEY_SPACE)){
        position.z += speed;
    }
    if (windowManager.isKeyDown(GLFW_KEY_LEFT_CONTROL)){
        position.z -= speed;
    }

    if (movement.length()!=0) {
        glm::normalize(movement);
    }

    glm::vec3 travelVector = glm::normalize(glm::vec3(lookNormal.x, lookNormal.y, 0)) * speed;

    position += glm::vec3(travelVector) * movement.x;

    travelVector = glm::rotate(travelVector, static_cast<float> (M_PI_2), glm::vec3(0,0,1));

    position += glm::vec3(travelVector) * movement.y;

//    if (windowManager.isKeyDown(GLFW_KEY_UP)){
//        mouseSensitivity+=0.1;
//    }
//    if (windowManager.isKeyDown(GLFW_KEY_DOWN)){
//        mouseSensitivity-=0.1;
//    }

    bool isFocused = glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE;

    if (!windowManager.isKeyDown(GLFW_KEY_TAB) && isFocused){
        if (glfwGetInputMode(window,GLFW_CURSOR) != GLFW_CURSOR_HIDDEN) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            glfwSetCursorPos(window,windowManager.getWindowWidth()/2,windowManager.getWindowHeight()/2);
        }
    } else {
        if (glfwGetInputMode(window,GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (isFocused && !windowManager.isKeyDown(GLFW_KEY_TAB)){

        double xPos, yPos;

        glfwGetCursorPos(window,&xPos,&yPos);

        double xDelta = (xPos - windowManager.getWindowWidth()/2);
        double yDelta = -(yPos - windowManager.getWindowHeight()/2);

        xDelta*=mouseSensitivity*windowManager.getLastFrameTime();
        yDelta*=mouseSensitivity*windowManager.getLastFrameTime();

        glfwSetCursorPos(window,windowManager.getWindowWidth()/2,windowManager.getWindowHeight()/2);

        pitch += yDelta;
        yaw += xDelta;

        pitch = Maths::clampBetween(pitch, (float) -(M_PI_2), (float) (M_PI_2), 0.0001f);

        generateNormal();

        infoChanged = true;

    }

}

void Camera::generateNormal() {

    float forwardFlatLength = std::cos(pitch);

    lookNormal = {
            std::sin(yaw) * forwardFlatLength,
            std::cos(yaw)*forwardFlatLength,
            std::sin(pitch)
    };

}

glm::mat4 Camera::getViewMatrix() {
    if(infoChanged){
        infoChanged = false;
        viewMatrix = glm::lookAt(position, position + lookNormal, up);
    }
    return viewMatrix;
}
