#ifndef MODEL_H
#define MODEL_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vulkan/vulkan.hpp>
#include <vector>

struct Vertex;

class Model {
public:
    const char* file = "";

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

    uint32_t indexCount;
    uint32_t vertexCount;
    std::vector<Vertex> vertices = {};
    std::vector<uint32_t> indices {};

};

struct Vertex{
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 texCoord{};

    static VkVertexInputBindingDescription getBindingDescription () {

        VkVertexInputBindingDescription bindingDescription = {};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof (Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;

    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions () {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};

        attributeDescriptions.resize(3);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = static_cast<uint32_t >(offsetof (Vertex, pos));

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = static_cast<uint32_t >(offsetof (Vertex, normal));

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = static_cast<uint32_t >(offsetof (Vertex, texCoord));

        return attributeDescriptions;

    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3> ()(vertex.pos) ^
                     (hash<glm::vec3> ()(vertex.normal) << 1)) >> 1) ^
                   (hash<glm::vec2> ()(vertex.texCoord) << 1);
        }
    };
}

#endif //MODEL_H
