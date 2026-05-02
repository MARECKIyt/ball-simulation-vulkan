#pragma once
#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FROCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace engine
{
    class Model
    {
        public:
        struct Vertex
        {
            glm::vec2 position;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        struct InstanceData
        {
            glm::vec3 color;
            glm::vec2 position;
            float size;
            float aspectRatio;
        };

        Model(Device& device, const std::vector<Vertex> &vertices);
        ~Model();
        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        void UpdateInstances(const std::vector<InstanceData>& instances);
        void BindAndDraw(VkCommandBuffer commandBuffer);

        private:
        void CreateVertexBuffers(const std::vector<Vertex> &vertices);
        void CreateInstanceBuffer(size_t maxInstances);

        Device& device;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        VkBuffer instanceBuffer;
        VkDeviceMemory instanceBufferMemory;
        uint32_t instanceCount;
        size_t maxInstanceCount;
    };
}