#include "UniformVariable.h"

 VkBuffer &UniformVariable::accessBuffer(uint32_t modelIndex, uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= buffers.size() || modelIndex < 0){
            output << "Index out of bounds error, accessing a per model buffer vector.\n"
                   << "Accessed array size: "
                   << buffers.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= buffers[modelIndex].size() || arrayIndex < 0){
            output << "Index out of bounds error, accessing a per array index buffer.\n"
                   << "Accessed array size: "
                   << buffers[modelIndex].size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return buffers[modelIndex][arrayIndex];
    #else
        return buffers[modelIndex][arrayIndex];
    #endif
}

 VkBuffer &UniformVariable::accessBuffer(uint32_t modelIndex) {
    return accessBuffer(modelIndex, 0);
}

 VkDeviceMemory &UniformVariable::accessMemory(uint32_t modelIndex, uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= memorys.size() || modelIndex < 0){
            output << "Index out of bounds error, accessing a per model memory vector.\n"
                   << "Accessed array size: "
                   << memorys.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= memorys[modelIndex].size() || arrayIndex < 0){
            output << "Index out of bounds error, accessing a per array index memory.\n"
                   << "Accessed array size: "
                   << memorys[modelIndex].size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return memorys[modelIndex][arrayIndex];
    #else
        return memorys[modelIndex][arrayIndex];
    #endif
}

 VkDeviceMemory &UniformVariable::accessMemory(uint32_t modelIndex) {
    return accessMemory(modelIndex, 0);
}

 VkDescriptorBufferInfo &UniformVariable::accessBufferInfo(uint32_t modelIndex, uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        std::stringstream output{};
        if (modelIndex >= bufferInfos.size() || modelIndex < 0){
            output << "Index out of bounds error, accessing a per model buffer info vector.\n"
                   << "Accessed array size: "
                   << bufferInfos.size()
                   << " at index: "
                   << modelIndex;
            throw std::runtime_error(output.str());
        } else if (arrayIndex >= bufferInfos[modelIndex].size() || arrayIndex < 0){
            output << "Index out of bounds error, accessing a per array index buffer info.\n"
                   << "Accessed array size: "
                   << bufferInfos[modelIndex].size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return bufferInfos[modelIndex][arrayIndex];
    #else
        return bufferInfos[modelIndex][arrayIndex];
    #endif
}

 VkDescriptorBufferInfo &UniformVariable::accessBufferInfo(uint32_t modelIndex) {
    return accessBufferInfo(modelIndex, 0);
}

 VkBuffer &StaticUniformVariable::accessBuffer(uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        if (arrayIndex >= buffers.size() || arrayIndex < 0){
            std::stringstream output{};
            output << "Index out of bounds error, accessing a model static per array index, buffer.\n"
                   << "Accessed array size: "
                   << buffers.size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return buffers[arrayIndex];
    #else
        return buffers[arrayIndex];
    #endif
}

 VkBuffer &StaticUniformVariable::accessBuffer() {
    return accessBuffer(0);
}

 VkDeviceMemory &StaticUniformVariable::accessMemory(uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        if (arrayIndex >= memorys.size() || arrayIndex < 0){
            std::stringstream output{};
            output << "Index out of bounds error, accessing a model static per array index, memory.\n"
                   << "Accessed array size: "
                   << buffers.size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return memorys[arrayIndex];
    #else
        return memorys[arrayIndex];
    #endif
}

VkDeviceMemory &StaticUniformVariable::accessMemory() {
    return accessMemory(0);
}

 VkDescriptorBufferInfo &StaticUniformVariable::accessBufferInfo(uint32_t arrayIndex) {
    #ifndef NDEBUG // If Debug
        if (arrayIndex >= bufferInfos.size() || arrayIndex < 0){
            std::stringstream output{};
            output << "Index out of bounds error, accessing a model static per array index, buffer info.\n"
                   << "Accessed array size: "
                   << bufferInfos.size()
                   << " at index: "
                   << arrayIndex;
            throw std::runtime_error(output.str());
        }
        return bufferInfos[arrayIndex];
    #else
        return bufferInfos[arrayIndex];
    #endif
}

 VkDescriptorBufferInfo &StaticUniformVariable::accessBufferInfo() {
    return accessBufferInfo(0);
}
