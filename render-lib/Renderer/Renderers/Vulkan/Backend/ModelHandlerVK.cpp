#include "ModelHandlerVK.h"
#include <cassert>
#include <filesystem>
#include <Utils/DebugHandler.h>
#include <Utils/FileReader.h>

#include "RenderDeviceVK.h"

namespace Renderer
{
    namespace Backend
    {
        ModelHandlerVK::ModelHandlerVK()
        {

        }

        ModelHandlerVK::~ModelHandlerVK()
        {

        }

        ModelID ModelHandlerVK::LoadModel(RenderDeviceVK* device, const ModelDesc& desc)
        {
            size_t nextHandle = _models.size();

            // Make sure we haven't exceeded the limit of the DepthImageID type, if this hits you need to change type of DepthImageID to something bigger
            assert(nextHandle < ModelID::MaxValue());
            using type = type_safe::underlying_type<ModelID>;

            Model model;
            TempModelData tempData;

            LoadFromFile(desc, tempData);
            InitializeModel(device, model, tempData);
                
            _models.push_back(model);
            return ModelID(static_cast<type>(nextHandle));
        }

        VkBuffer ModelHandlerVK::GetVertexBuffer(ModelID modelID)
        {
            using type = type_safe::underlying_type<ModelID>;

            // Lets make sure this id exists
            assert(_models.size() > static_cast<type>(modelID));
            return _models[static_cast<type>(modelID)].vertexBuffer;
        }

        u32 ModelHandlerVK::GetNumIndices(ModelID modelID)
        {
            using type = type_safe::underlying_type<ModelID>;

            // Lets make sure this id exists
            assert(_models.size() > static_cast<type>(modelID));
            return _models[static_cast<type>(modelID)].numIndices;
        }

        VkBuffer ModelHandlerVK::GetIndexBuffer(ModelID modelID)
        {
            using type = type_safe::underlying_type<ModelID>;

            // Lets make sure this id exists
            assert(_models.size() > static_cast<type>(modelID));
            return _models[static_cast<type>(modelID)].indexBuffer;
        }

        void ModelHandlerVK::LoadFromFile(const ModelDesc& desc, TempModelData& data)
        {
            // Open header
            std::filesystem::path path = std::filesystem::absolute(desc.path);
            FileReader file(path.string(), path.filename().string());
            if (!file.Open())
            {
                NC_LOG_FATAL("Could not open Model file %s", desc.path.c_str());
            }

            assert(file.Length() > sizeof(NovusTypeHeader));

            std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<32768>();
            file.Read(*buffer, file.Length());

            // Read header
            NovusTypeHeader header;
            if (!buffer->Get<NovusTypeHeader>(header))
            {
                NC_LOG_FATAL("Model file %s did not have a valid NovusTypeHeader", desc.path.c_str());
            }

            if (header != EXPECTED_TYPE_HEADER)
            {
                if (header.typeID != EXPECTED_TYPE_HEADER.typeID)
                {
                    NC_LOG_FATAL("Model file %s had an invalid TypeID in its NovusTypeHeader, %u != %u", header.typeID, EXPECTED_TYPE_HEADER.typeID);
                }
                if (header.typeVersion != EXPECTED_TYPE_HEADER.typeVersion)
                {
                    NC_LOG_FATAL("Model file %s had an invalid TypeVersion in its NovusTypeHeader, %u != %u", header.typeVersion, EXPECTED_TYPE_HEADER.typeVersion);
                }
            }

            // Read vertex count
            u32 vertexCount;
            if (!buffer->GetU32(vertexCount))
            {
                NC_LOG_FATAL("Model file %s did not have a valid vertexCount", desc.path.c_str());
            }
            
            // Read vertices
            data.vertices.resize(vertexCount);
            
            for (u32 i = 0; i < vertexCount; i++)
            {
                if (!buffer->Get<Vector3>(data.vertices[i].pos))
                {
                    NC_LOG_FATAL("Model file %s failed to read vertex %u position", desc.path.c_str(), i);
                }

                if (!buffer->Get<Vector3>(data.vertices[i].normal))
                {
                    NC_LOG_FATAL("Model file %s failed to read vertex %u normal", desc.path.c_str(), i);
                }

                if (!buffer->Get<Vector2>(data.vertices[i].texCoord))
                {
                    NC_LOG_FATAL("Model file %s failed to read vertex %u texCoord", desc.path.c_str(), i);
                }
            }

            // Read index type
            if (!buffer->GetI32(data.indexType))
            {
                NC_LOG_FATAL("Model file %s did not have a valid indexType", desc.path.c_str());
            }

            // Read index count
            u32 indexCount;
            if (!buffer->GetU32(indexCount))
            {
                NC_LOG_FATAL("Model file %s did not have a valid indexCount", desc.path.c_str());
            }

            // Read indices
            data.indices.resize(indexCount);

            for (u32 i = 0; i < indexCount; i++)
            {
                if (!buffer->GetI16(data.indices[i]))
                {
                    NC_LOG_FATAL("Model file %s failed to read index %u", desc.path.c_str(), i);
                }
            }
        }

        void ModelHandlerVK::InitializeModel(RenderDeviceVK* device, Model& model, const TempModelData& data)
        {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;

            model.numIndices = static_cast<u32>(data.indices.size());

            // -- Create vertex buffer --
            VkDeviceSize vertexBufferSize = sizeof(data.vertices[0]) * data.vertices.size();

            // Create a staging buffer
            device->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            // Copy our vertex data into the staging buffer
            void* vertexData;
            vkMapMemory(device->_device, stagingBufferMemory, 0, vertexBufferSize, 0, &vertexData);
            memcpy(vertexData, data.vertices.data(), (size_t)vertexBufferSize);
            vkUnmapMemory(device->_device, stagingBufferMemory);

            // Create our vertex buffer
            device->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.vertexBuffer, model.vertexBufferMemory);
            // Copy the vertex data from our staging buffer to our vertex buffer
            device->CopyBuffer(stagingBuffer, model.vertexBuffer, vertexBufferSize);

            // Destroy and free our staging buffer
            vkDestroyBuffer(device->_device, stagingBuffer, nullptr);
            vkFreeMemory(device->_device, stagingBufferMemory, nullptr);

            // -- Create index buffer --
            VkDeviceSize indexBufferSize = sizeof(data.indices[0]) * data.indices.size();

            // Create a staging buffer
            device->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            // Copy our index data into the staging buffer
            void* indexData;
            vkMapMemory(device->_device, stagingBufferMemory, 0, indexBufferSize, 0, &indexData);
            memcpy(indexData, data.indices.data(), (size_t)indexBufferSize);
            vkUnmapMemory(device->_device, stagingBufferMemory);

            // Create our index buffer
            device->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.indexBuffer, model.indexBufferMemory);

            // Copy the index data from our staging buffer to our index buffer
            device->CopyBuffer(stagingBuffer, model.indexBuffer, indexBufferSize);

            // Destroy and free our staging buffer
            vkDestroyBuffer(device->_device, stagingBuffer, nullptr);
            vkFreeMemory(device->_device, stagingBufferMemory, nullptr);

            // -- Create attribute descriptor --
            model.attributeDescriptions.resize(3);

            // Position
            model.attributeDescriptions[0].binding = 0;
            model.attributeDescriptions[0].location = 0;
            model.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            model.attributeDescriptions[0].offset = offsetof(Vertex, pos);

            // Normal
            model.attributeDescriptions[1].binding = 0;
            model.attributeDescriptions[1].location = 1;
            model.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            model.attributeDescriptions[1].offset = offsetof(Vertex, normal);

            // Texcoord
            model.attributeDescriptions[2].binding = 0;
            model.attributeDescriptions[2].location = 2;
            model.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            model.attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        }
    }
}