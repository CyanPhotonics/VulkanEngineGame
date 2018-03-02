#include "WindowManager.h"
#include "../testing/EngineTester.h"

void WindowManager::initWindow(char *title, int width, int height, EngineTester *engineTester, bool fullscreen) {

    glfwInit();

    if (fullscreen) {
        const GLFWvidmode *glfWvidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        this->width = glfWvidmode->width;
        this->height = glfWvidmode->height;
    } else {
        this->width = width;
        this->height = height;
    }

	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_VISIBLE, GLFW_FALSE);
    if (fullscreen) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

	vulkanManager->window = glfwCreateWindow (this->width, this->height, title, nullptr, nullptr);

	glfwSetKeyCallback (vulkanManager->window, keyCallback);

	glfwSetWindowUserPointer (vulkanManager->window, engineTester);
	//glfwSetWindowSizeCallback (vulkanManager->window, onWindowResize);
    glfwSetFramebufferSizeCallback(vulkanManager->window, onWindowResize);

	centreWindow();

}

void WindowManager::createSurface () {
	if (glfwCreateWindowSurface (vulkanManager->instance, vulkanManager->window, nullptr, &vulkanManager->surface) != VK_SUCCESS) {
		throw std::runtime_error ("Failed to create surface");
	}
}

void WindowManager::centreWindow () {

	const GLFWvidmode* vidMode = glfwGetVideoMode (glfwGetPrimaryMonitor ());

	glfwSetWindowPos (vulkanManager->window,
		(int)(0.5 * vidMode->width - 0.5 * width),
		(int)(0.5 * vidMode->height - 0.5 * height)
	);

}

void WindowManager::keyCallback (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose (window, GLFW_TRUE);

}

void WindowManager::onWindowResize (GLFWwindow* window, int newWidth, int newHeight) {
	if (newWidth == 0 || newHeight == 0) return;

    auto* app = reinterpret_cast<EngineTester*>(glfwGetWindowUserPointer (window));

    app->updateWindowSize(newWidth, newHeight);

	app->cleanUpSwapChain();
	app->createSwapChain ();
}

bool WindowManager::isOpen () {
	return !glfwWindowShouldClose (vulkanManager->window);
}

void WindowManager::update () {
	glfwPollEvents ();

    sync();

    std::ostringstream strs;
    strs << "Vulkan Engine - Last frame time: " << lastFrameTime << " - Effective FPS: " << (1.0/lastFrameTime);
    glfwSetWindowTitle(vulkanManager->window, strs.str().c_str());
    strs << "\n";
	//printf(strs.str().c_str());
}

void WindowManager::sync() {

    double time;

	if (lastTime == 0)
        lastTime = glfwGetTime();

    while ((time = glfwGetTime()) - lastTime <= 1.0/fpsLimit){
        std::this_thread::yield();
    }

    lastFrameTime = time - lastTime;
    lastTime = time;

}

void WindowManager::showWindow () {
	glfwShowWindow (vulkanManager->window);
}

std::vector<const char*> WindowManager::getRequiredExtensions () {

	std::vector<const char*> requiredExtensions;

	uint32_t extensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions (&extensionCount);

	for (uint32_t i = 0; i < extensionCount; i++) {
		requiredExtensions.push_back (glfwExtensions[i]);
	}

	return requiredExtensions;

}

double WindowManager::getLastFrameTime () {
	return lastFrameTime;
}

int WindowManager::getWindowWidth () {
	return width;
}

int WindowManager::getWindowHeight () {
	return height;
}

glm::ivec2 WindowManager::getWindowSize () {
	return { width, height };
}

bool WindowManager::isKeyDown (int keyCode) {
    return glfwGetKey(vulkanManager->window, keyCode) == GLFW_PRESS;
}

void WindowManager::cleanUp () {

	vkDestroySurfaceKHR (vulkanManager->instance, vulkanManager->surface, nullptr);
	vulkanManager->surface = VK_NULL_HANDLE;

	glfwDestroyWindow (vulkanManager->window);
	vulkanManager->window = nullptr;
	glfwTerminate ();

}
