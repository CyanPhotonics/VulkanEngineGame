#ifndef RENDER_PASS_MANAGER_H
#define RENDER_PASS_MANAGER_H

#include <vector>
#include <stdexcept>
#include <array>

#include "../vulkanSetup/VulkanManager.h"

class RenderPassManager {
private:
    VulkanManager* vulkanManager;
public:
    explicit RenderPassManager(VulkanManager* vulkanManager) : vulkanManager(vulkanManager) {}
    void createRenderPassesForSwapChain();
    void cleanUp();
};

#endif //RENDER_PASS_MANAGER_H
