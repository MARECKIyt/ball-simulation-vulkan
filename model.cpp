#include "model.hpp"

#include <cassert>
#include <cstring>

namespace engine
{
    Model::Model(Device& device, const std::vector<Vertex> &vertices) : device{device}
    {
        CreateVertexBuffers(vertices);
        CreateInstanceBuffer(65536); // rezerwacja miejsca dla maksymalnej liczby kulek czyli 2^16 bo więcej raczej nikt nie będzie chiał symulować
    }

    Model::~Model()
    {
        vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
        vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
        vkDestroyBuffer(device.device(), instanceBuffer, nullptr);
        vkFreeMemory(device.device(), instanceBufferMemory, nullptr);
    }

    void Model::CreateVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "muszą być conajmniej 3 wierzchołki");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);

        void *data;
        vkMapMemory(device.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(device.device(), vertexBufferMemory);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindings(2);

        bindings[0].binding = 0;
        bindings[0].stride = sizeof(Vertex);
        bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindings[1].binding = 1;
        bindings[1].stride = sizeof(InstanceData);
        bindings[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return bindings;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attrs(4);

        attrs[0].binding = 0;
        attrs[0].location = 0;
        attrs[0].format = VK_FORMAT_R32G32_SFLOAT;
        attrs[0].offset = offsetof(Vertex, position);

        attrs[1].binding = 1;
        attrs[1].location = 1;
        attrs[1].format = VK_FORMAT_R32G32_SFLOAT;
        attrs[1].offset = offsetof(InstanceData, position);

        attrs[2].binding = 1;
        attrs[2].location = 2;
        attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attrs[2].offset = offsetof(InstanceData, size);

        attrs[3].binding = 1;
        attrs[3].location = 3;
        attrs[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrs[3].offset = offsetof(InstanceData, color);

        return attrs;
    }

    void Model::CreateInstanceBuffer(size_t maxInstances)
    {
        maxInstanceCount = maxInstances;
        VkDeviceSize bufferSize = sizeof(InstanceData) * maxInstances;
        device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            instanceBuffer,
            instanceBufferMemory
        );
    }

    void Model::UpdateInstances(const std::vector<InstanceData>& instances)
    {
        instanceCount = static_cast<uint32_t>(
            std::min(instances.size(), maxInstanceCount)
        );
        if (instanceCount == 0) return;

        void* data;
        vkMapMemory(device.device(), instanceBufferMemory, 0,
                    sizeof(InstanceData) * instanceCount, 0, &data);
        memcpy(data, instances.data(), sizeof(InstanceData) * instanceCount);
        vkUnmapMemory(device.device(), instanceBufferMemory);
    }

    void Model::BindAndDraw(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer, instanceBuffer};
        VkDeviceSize offsets[] = {0, 0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
    }
}