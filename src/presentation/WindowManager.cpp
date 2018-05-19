#include "WindowManager.h"
#include "../testing/EngineTester.h"

void WindowManager::initWindow(char *title, int width, int height, EngineTester *engineTester, bool fullscreen) {

	this->fullscreen = fullscreen;

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

    if (vulkanManager->window == nullptr){
        return;
    }

	glfwSetKeyCallback (vulkanManager->window, keyCallback);

	glfwSetWindowUserPointer (vulkanManager->window, engineTester);
	//glfwSetWindowSizeCallback (vulkanManager->window, onWindowResize);
    glfwSetFramebufferSizeCallback(vulkanManager->window, onWindowResize);

	centreWindow();

}

void WindowManager::setFullscreen(bool fullscreen){

    if (this->fullscreen == fullscreen) return;

    this->fullscreen = fullscreen;

    static uint32_t oldWidth = 1280;
    static uint32_t oldHeight = 720;

    static int oldX = -1;
    static int oldY = -1;

    if (fullscreen){

        oldWidth = static_cast<uint32_t>(width);
        oldHeight = static_cast<uint32_t>(height);

        glfwGetWindowPos(vulkanManager->window, &oldX, &oldY);

        const GLFWvidmode* glfwVidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        glfwSetWindowMonitor(vulkanManager->window, glfwGetPrimaryMonitor(), 0,0,glfwVidMode->width, glfwVidMode->height, glfwVidMode->refreshRate);

    } else {

        bool centre = false;

        if(oldX == -1 || oldY == -1){
            oldX = 0, oldY = 0;
            centre = true;
        }

        glfwSetWindowMonitor(vulkanManager->window, nullptr, oldX, oldY, oldWidth, oldHeight, GLFW_DONT_CARE);

        if (centre) centreWindow();

    }

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

	if (key == GLFW_KEY_F11 && action == GLFW_RELEASE){
        auto* app = reinterpret_cast<EngineTester*>(glfwGetWindowUserPointer (window));

        app->setFullscreen();
	}

}

void WindowManager::onWindowResize (GLFWwindow* window, int newWidth, int newHeight) {
	if (newWidth == 0 || newHeight == 0) return;

    auto* app = reinterpret_cast<EngineTester*>(glfwGetWindowUserPointer (window));

    app->updateWindowSize(newWidth, newHeight);

//	app->cleanUpSwapChain();
//	app->createSwapChain ();
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
	glfwFocusWindow(vulkanManager->window);
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

bool WindowManager::isFullscreen() {
    return fullscreen;
}

bool WindowManager::shouldNotRender() {

    int width, height;
    glfwGetFramebufferSize(vulkanManager->window, &width, &height);

    return (isFullscreen() &&  !glfwGetWindowAttrib(vulkanManager->window, GLFW_FOCUSED)) || glfwGetWindowAttrib(vulkanManager->window, GLFW_ICONIFIED) || width <= 0 || height <= 0                    ;

}
