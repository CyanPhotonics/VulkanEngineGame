//#define TINYOBJLOADER_IMPLEMENTATION
//
//#include <tiny_obj_loader.h>

#include "OBJ_Loader.h"

#include "ModelManager.h"

Model ModelManager::loadIntoLocal(const char *file) {

    //Check if model already loaded, and return it if it is

    if (models.count(file) != 0) {
        return models.find(file)->second;
    }

    //Create model object

    Model model{};

    model.file = file;

    //Load in model

    objl::Loader Loader;

    std::string filePath = modelFolder;
    filePath += file;

    bool loaded = Loader.LoadFile(filePath);

    if (!loaded)
        throw std::runtime_error("Failed to load model file: " + filePath);

    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

    for (const auto &mesh : Loader.LoadedMeshes) {
        for (const auto &index : mesh.Indices) {
            Vertex vertex = {};

            vertex.pos = {
                    mesh.Vertices[index].Position.X,
                    mesh.Vertices[index].Position.Y,
                    mesh.Vertices[index].Position.Z
            };

            vertex.texCoord = {
                    mesh.Vertices[index].TextureCoordinate.X,
                    1.0f - mesh.Vertices[index].TextureCoordinate.Y
            };

            vertex.normal = {
                    mesh.Vertices[index].Normal.X,
                    mesh.Vertices[index].Normal.Y,
                    mesh.Vertices[index].Normal.Z
            };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
                model.vertices.push_back(vertex);
            }

            model.indices.push_back(uniqueVertices[vertex]);
        }
    }

    model.indexCount = static_cast<uint32_t>(model.indices.size());
    model.vertexCount = static_cast<uint32_t>(model.vertices.size());

    models[file] = model;

    return model;

}

//Model ModelManager::loadIntoLocal(const char *file) {
//
//    if (models.count(file) != 0){
//        return models.find(file)->second;
//    }
//
//    Model model{};
//
//    model.file = file;
//
//    tinyobj::attrib_t attrib;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string error;
//
//    std::string filePath = modelFolder;
//    filePath += file;
//
//    if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, filePath.c_str())){
//        throw std::runtime_error(error);
//    }
//
//    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};
//
//    for (const auto& shape : shapes) {
//        for (const auto& index : shape.mesh.indices) {
//            Vertex vertex = {};
//
//            vertex.pos = {
//                    attrib.vertices[3 * index.vertex_index + 0],
//                    attrib.vertices[3 * index.vertex_index + 1],
//                    attrib.vertices[3 * index.vertex_index + 2]
//            };
//
//            vertex.texCoord = {
//                    attrib.texcoords[2 * index.texcoord_index + 0],
//                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
//            };
//
////            vertex.normal = {
////                    attrib.normals[3 * index.normal_index + 0],
////                    attrib.normals[3 * index.normal_index + 1],
////                    attrib.normals[3 * index.normal_index + 2]
////            };
//
//            vertex.normal = { 1.0f,1.0f,1.0f };
//
//            if (uniqueVertices.count (vertex) == 0) {
//                uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size ());
//                model.vertices.push_back (vertex);
//            }
//
//            model.indices.push_back (uniqueVertices[vertex]);
//        }
//    }
//
//    model.indexCount = static_cast<uint32_t>(model.indices.size());
//    model.vertexCount = static_cast<uint32_t>(model.vertices.size());
//
//    models[file] = model;
//
//    return model;
//
//}

void ModelManager::loadIntoDevice(Model &model) {

    if (model.vertexBuffer != VK_NULL_HANDLE){
        return;
    }

    createVertexBuffer(model);
    createIndexBuffer(model);

    models[model.file] = model;

}

void ModelManager::createVertexBuffer(Model &model) {

    VkDeviceSize bufferSize = sizeof (model.vertices[0]) * model.vertices.size ();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    memoryUtility->createBuffer (bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory (vulkanManager->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy (data, model.vertices.data (), (size_t)bufferSize);
    vkUnmapMemory (vulkanManager->device, stagingBufferMemory);

    memoryUtility->createBuffer (bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.vertexBuffer, model.vertexBufferMemory);

    memoryUtility->copyBuffer (stagingBuffer, model.vertexBuffer, bufferSize);

    vkDestroyBuffer (vulkanManager->device, stagingBuffer, nullptr);
    vkFreeMemory (vulkanManager->device, stagingBufferMemory, nullptr);

}

void ModelManager::createIndexBuffer(Model &model) {

    VkDeviceSize bufferSize = sizeof (model.indices[0]) * model.indices.size ();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    memoryUtility->createBuffer (bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory (vulkanManager->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy (data, model.indices.data (), (size_t)bufferSize);
    vkUnmapMemory (vulkanManager->device, stagingBufferMemory);

    memoryUtility->createBuffer (bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.indexBuffer, model.indexBufferMemory);

    memoryUtility->copyBuffer (stagingBuffer, model.indexBuffer, bufferSize);

    vkDestroyBuffer (vulkanManager->device, stagingBuffer, nullptr);
    vkFreeMemory (vulkanManager->device, stagingBufferMemory, nullptr);

}

void ModelManager::unloadFromDevice(Model &model) {

    model = models[model.file];

    if (model.vertexBuffer == VK_NULL_HANDLE){
        return;
    }

    vkDestroyBuffer(vulkanManager->device, model.vertexBuffer, nullptr);
    vkFreeMemory(vulkanManager->device, model.vertexBufferMemory, nullptr);

    vkDestroyBuffer(vulkanManager->device, model.indexBuffer, nullptr);
    vkFreeMemory(vulkanManager->device, model.indexBufferMemory, nullptr);

    model.vertexBuffer = VK_NULL_HANDLE;
    model.vertexBufferMemory = VK_NULL_HANDLE;
    model.indexBuffer = VK_NULL_HANDLE;
    model.indexBufferMemory = VK_NULL_HANDLE;

    models[model.file] = model;

}

void ModelManager::unloadFromLocal(Model &model) {

    model = models[model.file];

    model.vertices.clear();
    model.indices.clear();

    models[model.file] = model;

}

void ModelManager::cleanUpModels() {

    for (auto modelIter : models){
        unloadFromDevice(modelIter.second);
        unloadFromLocal(modelIter.second);
    }

}
