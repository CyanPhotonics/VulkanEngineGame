#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#define GLFW_INCLUDE_VULKAN
#define GLFW_NO_DISPLAY
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <thread>

#include "../vulkanSetup/VulkanManager.h"

class EngineTester;
class WindowManager {

private:
    VulkanManager* vulkanManager;

    int width = 720, height = 480;
	double lastTime = 0, lastFrameTime = 0;

	void centreWindow ();

	static void keyCallback (GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onWindowResize (GLFWwindow* window, int newWidth, int newHeight);

	void sync();

    double fpsLimit = 120;

public:
	explicit WindowManager (VulkanManager *vulkanManger) : vulkanManager(vulkanManger) {}

	void initWindow(char *title, int width, int height, EngineTester *engineTester, bool fullscreen);
	void createSurface ();
	void showWindow ();
	bool isOpen ();
	void update ();

	std::vector<const char*> getRequiredExtensions ();
	double getLastFrameTime ();
	int getWindowWidth ();
	int getWindowHeight ();
	glm::ivec2 getWindowSize ();

	bool isKeyDown (int keyCode);
	bool fullscreen = false;

	void cleanUp ();

    void updateWindowSize(int width, int height){
        this->width = width;
        this->height = height;
    }
};

#endif // !WINDOW_MANAGER_H
