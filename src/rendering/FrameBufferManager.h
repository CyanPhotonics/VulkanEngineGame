#ifndef FRAME_BUFFER_MANAGER_H
#define FRAME_BUFFER_MANAGER_H

#include <vector>
#include <array>

#include "../vulkanSetup/VulkanManager.h"

#include "../utility/CollectionUtility.h"

class FrameBufferManager {
private:
    VulkanManager* vulkanManager;

    std::vector<VkFramebuffer> frameBuffers{};

public:
    explicit FrameBufferManager(VulkanManager *vulkanManager){ this->vulkanManager = vulkanManager; }

    void createFrameBuffersWithSwapChain(std::vector<VkImageView> extraAttachments);

    const std::vector<VkFramebuffer> getFrameBuffers() const;

    void cleanUp();
};

#endif //FRAME_BUFFER_MANAGER_H
