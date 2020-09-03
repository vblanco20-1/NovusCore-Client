#include "NM2Renderer.h"
#include "DebugRenderer.h"
#include "../Utils/ServiceLocator.h"
#include "../Rendering/NM2/NM2.h"

#include <filesystem>
#include <GLFW/glfw3.h>

#include <InputManager.h>
#include <Renderer/Renderer.h>
#include <Utils/FileReader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "../ECS/Components/Singletons/TextureSingleton.h"

#include <tracy/TracyVulkan.hpp>

namespace fs = std::filesystem;

NM2Renderer::NM2Renderer(Renderer::Renderer* renderer, DebugRenderer* debugRenderer)
    : _renderer(renderer)
    , _debugRenderer(debugRenderer)
{
    CreatePermanentResources();
}

NM2Renderer::~NM2Renderer()
{

}

void NM2Renderer::Update(f32 deltaTime)
{

}

void NM2Renderer::AddNM2Pass(Renderer::RenderGraph* renderGraph, Renderer::DescriptorSet* globalDescriptorSet, Renderer::ImageID renderTarget, Renderer::DepthImageID depthTarget, u8 frameIndex)
{
    struct NM2PassData
    {
        Renderer::RenderPassMutableResource mainColor;
        Renderer::RenderPassMutableResource mainDepth;
    };

    const auto setup = [=](NM2PassData& data, Renderer::RenderGraphBuilder& builder)
    {
        data.mainColor = builder.Write(renderTarget, Renderer::RenderGraphBuilder::WriteMode::WRITE_MODE_RENDERTARGET, Renderer::RenderGraphBuilder::LoadMode::LOAD_MODE_CLEAR);
        data.mainDepth = builder.Write(depthTarget, Renderer::RenderGraphBuilder::WriteMode::WRITE_MODE_RENDERTARGET, Renderer::RenderGraphBuilder::LoadMode::LOAD_MODE_CLEAR);

        return true; // Return true from setup to enable this pass, return false to disable it
    };

    const auto execute = [=](NM2PassData& data, Renderer::RenderGraphResources& resources, Renderer::CommandList& commandList)
    {
        GPU_SCOPED_PROFILER_ZONE(commandList, NM2Pass);

        Renderer::GraphicsPipelineDesc pipelineDesc;
        resources.InitializePipelineDesc(pipelineDesc);

        // Shaders
        Renderer::VertexShaderDesc vertexShaderDesc;
        vertexShaderDesc.path = "Data/shaders/nm2.vs.hlsl.spv";
        pipelineDesc.states.vertexShader = _renderer->LoadShader(vertexShaderDesc);

        Renderer::PixelShaderDesc pixelShaderDesc;
        pixelShaderDesc.path = "Data/shaders/nm2.ps.hlsl.spv";
        pipelineDesc.states.pixelShader = _renderer->LoadShader(pixelShaderDesc);

        // Depth state
        pipelineDesc.states.depthStencilState.depthEnable = true;
        pipelineDesc.states.depthStencilState.depthWriteEnable = true;
        pipelineDesc.states.depthStencilState.depthFunc = Renderer::ComparisonFunc::COMPARISON_FUNC_LESS;

        // Rasterizer state
        pipelineDesc.states.rasterizerState.cullMode = Renderer::CullMode::CULL_MODE_NONE; //Renderer::CullMode::CULL_MODE_BACK;
        pipelineDesc.states.rasterizerState.frontFaceMode = Renderer::FrontFaceState::FRONT_FACE_STATE_COUNTERCLOCKWISE;

        // Render targets
        pipelineDesc.renderTargets[0] = data.mainColor;
        pipelineDesc.depthStencil = data.mainDepth;

        // Set pipeline
        Renderer::GraphicsPipelineID pipeline = _renderer->CreatePipeline(pipelineDesc); // This will compile the pipeline and return the ID, or just return ID of cached pipeline
        commandList.BeginPipeline(pipeline);

        commandList.BindDescriptorSet(Renderer::DescriptorSetSlot::GLOBAL, globalDescriptorSet, frameIndex);

        for (LoadedNM2& loadedNM2 : _loadedNM2s)
        {
            commandList.PushMarker("NM2", Color::White);

            _passDescriptorSet.Bind("_instanceData", loadedNM2.instanceBuffer);
            _passDescriptorSet.Bind("_materialData", loadedNM2.materialsBuffer);
            _passDescriptorSet.Bind("_textures", _m2Textures);
            commandList.BindDescriptorSet(Renderer::DescriptorSetSlot::PER_PASS, &_passDescriptorSet, frameIndex);

            for (Mesh& mesh : loadedNM2.meshes)
            {
                _meshDescriptorSet.Bind("_vertexPositions", mesh.vertexPositionsBuffer);
                _meshDescriptorSet.Bind("_vertexUVs", mesh.vertexUVsBuffer);
                commandList.BindDescriptorSet(Renderer::DescriptorSetSlot::PER_DRAW, &_meshDescriptorSet, frameIndex);

                size_t numSubMeshes = mesh.subMeshes.size();
                if (_debugSubMeshRendering)
                {
                    numSubMeshes = glm::min(_numSubMeshesToRender, numSubMeshes);
                    _numSubMeshesToRender = numSubMeshes;
                }

                for (size_t i = _startSubMeshIndexToRender; i < numSubMeshes; i++)
                {
                    SubMesh& subMesh = mesh.subMeshes[i];

                    commandList.PushConstant(&subMesh.materialNum, 0, sizeof(u32));

                    commandList.SetIndexBuffer(subMesh.indexBuffer, Renderer::IndexFormat::UInt16);
                    commandList.DrawIndexed(subMesh.indexCount, loadedNM2.numInstances, 0, 0, 0);
                }
            }

            commandList.PopMarker();
        }

        commandList.EndPipeline(pipeline);
    };

    renderGraph->AddPass<NM2PassData>("NM2 Pass", setup, execute);
}

void NM2Renderer::CreatePermanentResources()
{
    InputManager* inputManager = ServiceLocator::GetInputManager();
    inputManager->RegisterKeybind("M2Renderer Draw Submesh Debug Toggle", GLFW_KEY_END, KEYBIND_ACTION_PRESS, KEYBIND_MOD_NONE, [&](Window* window, std::shared_ptr<Keybind> keybind)
    {
        _debugSubMeshRendering = !_debugSubMeshRendering;
        return true;
    });
    inputManager->RegisterKeybind("M2Renderer Draw Submesh Count Increase", GLFW_KEY_INSERT, KEYBIND_ACTION_PRESS, KEYBIND_MOD_NONE, [&](Window* window, std::shared_ptr<Keybind> keybind)
    {
        _numSubMeshesToRender++;
        return true;
    });
    inputManager->RegisterKeybind("M2Renderer Draw Submesh Count Decrease", GLFW_KEY_DELETE, KEYBIND_ACTION_PRESS, KEYBIND_MOD_NONE, [&](Window* window, std::shared_ptr<Keybind> keybind)
    {
        _numSubMeshesToRender = glm::max(_numSubMeshesToRender - 1, static_cast<size_t>(0));
        return true;
    });
    inputManager->RegisterKeybind("M2Renderer Draw Submesh Index Increase", GLFW_KEY_UP, KEYBIND_ACTION_PRESS, KEYBIND_MOD_NONE, [&](Window* window, std::shared_ptr<Keybind> keybind)
    {
        _startSubMeshIndexToRender++;
        return true;
    });
    inputManager->RegisterKeybind("M2Renderer Draw Submesh Index Decrease", GLFW_KEY_DOWN, KEYBIND_ACTION_PRESS, KEYBIND_MOD_NONE, [&](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (_startSubMeshIndexToRender > 0)
            _startSubMeshIndexToRender--;

        return true;
    });

    Renderer::SamplerDesc samplerDesc;
    samplerDesc.enabled = true;
    samplerDesc.filter = Renderer::SamplerFilter::SAMPLER_FILTER_MIN_MAG_MIP_LINEAR;//Renderer::SamplerFilter::SAMPLER_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    samplerDesc.addressU = Renderer::TextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.addressV = Renderer::TextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.addressW = Renderer::TextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.shaderVisibility = Renderer::ShaderVisibility::SHADER_VISIBILITY_PIXEL;

    _sampler = _renderer->CreateSampler(samplerDesc);

    _passDescriptorSet.SetBackend(_renderer->CreateDescriptorSetBackend());
    _passDescriptorSet.Bind("_sampler", _sampler);

    /*u32 objectID = 0;
    //if (LoadNM2("CHARACTER\\Scourge\\Male\\ScourgeMale.nm2", objectID))
    //if (LoadNM2("Creature\\Anduin\\Anduin.nm2", objectID))
    if (LoadNM2("World\\Expansion02\\Doodads\\IceCrown\\Weapon\\IceCrown_Axe_ShadowsEdge.nm2", objectID))
    {
        LoadedNM2& nm2 = _loadedNM2s[objectID];

        // Create staging buffer
        const u32 bufferSize = sizeof(Instance);
        Renderer::BufferDesc desc;
        desc.name = "InstanceStaging";
        desc.size = bufferSize;
        desc.usage = Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_SOURCE;
        desc.cpuAccess = Renderer::BufferCPUAccess::WriteOnly;

        Renderer::BufferID stagingBuffer = _renderer->CreateBuffer(desc);

        // Upload to staging buffer
        Instance* dst = reinterpret_cast<Instance*>(_renderer->MapBuffer(stagingBuffer));

        vec3 pos = vec3(-8000, 100.f, 1600.f);
        mat4x4 rotationMatrix = glm::eulerAngleXYZ(glm::radians(0.f), glm::radians(0.f), glm::radians(0.f));

        dst->instanceMatrix = glm::translate(mat4x4(1.0f), pos) * rotationMatrix;
        _renderer->UnmapBuffer(stagingBuffer);

        u32 dstOffset = sizeof(Instance) * nm2.numInstances++;

        // Queue destroy staging buffer
        _renderer->QueueDestroyBuffer(stagingBuffer);

        // Copy from staging buffer to buffer
        _renderer->CopyBuffer(nm2.instanceBuffer, dstOffset, stagingBuffer, 0, bufferSize);
    }*/
}

bool NM2Renderer::LoadNM2(std::string nm2Name, u32& objectID)
{
    u32 nameHash = StringUtils::fnv1a_32(nm2Name.c_str(), nm2Name.length());

    auto it = _nameHashToIndexMap.find(nameHash);
    if (it != _nameHashToIndexMap.end())
    {
        objectID = it->second;
        return true;
    }

    static u32 objectCount = 0;
    NC_LOG_MESSAGE("Loading NM2 %u", objectCount);
    objectCount++;

    u32 nextID = static_cast<u32>(_loadedNM2s.size());
    LoadedNM2& loadedNM2 = _loadedNM2s.emplace_back();
    loadedNM2.debugName = nm2Name;

    if (!StringUtils::EndsWith(nm2Name, ".nm2"))
    {
        NC_LOG_FATAL("Tried to call 'LoadNM2' with a reference to a file that didn't end with '.nm2'");
        return false;
    }

    fs::path nm2Path = "Data/extracted/NM2/" + nm2Name;
    nm2Path.make_preferred();
    nm2Path = fs::absolute(nm2Path);

    FileReader nm2File(nm2Path.string(), nm2Path.filename().string());
    if (!nm2File.Open())
    {
        NC_LOG_FATAL("Failed to open NM2 file: %s", nm2Path.string().c_str());
        return false;
    }

    Bytebuffer nm2Buffer(nullptr, nm2File.Length());
    nm2File.Read(&nm2Buffer, nm2Buffer.size);
    nm2File.Close();

    NM2::NM2Root nm2;

    if (!nm2Buffer.Get(nm2.header))
        return false;

    u32 numVertices = 0;
    if (!nm2Buffer.GetU32(numVertices))
        return false;

    if (numVertices > 0)
    {
        nm2.vertices.resize(numVertices);
        nm2Buffer.GetBytes(reinterpret_cast<u8*>(nm2.vertices.data()), numVertices * sizeof(NM2::M2Vertex));
    }

    u32 numTextures = 0;
    if (!nm2Buffer.GetU32(numTextures))
        return false;

    if (numTextures > 0)
    {
        nm2.textures.resize(numTextures);
        nm2Buffer.GetBytes(reinterpret_cast<u8*>(nm2.textures.data()), numTextures * sizeof(NM2::M2Texture));
    }

    u32 numTextureCombos = 0;
    if (!nm2Buffer.GetU32(numTextureCombos))
        return false;

    if (numTextureCombos > 0)
    {
        nm2.textureCombos.resize(numTextureCombos);
        if (!nm2Buffer.GetBytes(reinterpret_cast<u8*>(nm2.textureCombos.data()), numTextureCombos * sizeof(u16)))
            return false;
    }

    u32 numSkins = 0;
    if (!nm2Buffer.GetU32(numSkins))
        return false;

    std::vector<Material> materials;
    materials.reserve(8);

    if (numSkins > 0)
    {
        nm2.skins.reserve(numSkins);

        for (u32 i = 0; i < numSkins; i++)
        {
            // Create mesh, each MapObject becomes one Mesh in loadedMapObject
            Mesh& mesh = loadedNM2.meshes.emplace_back();
            NM2::M2Skin& skin = nm2.skins.emplace_back();
            std::vector<vec3> vertexPositions;
            std::vector<vec2> uvCoordinates;

            if (!nm2Buffer.GetU32(skin.token))
                return false;

            u32 numSkinVertexIndexes = 0;
            if (!nm2Buffer.GetU32(numSkinVertexIndexes))
                return false;

            if (numSkinVertexIndexes > 0)
            {
                skin.vertexIndexes.resize(numSkinVertexIndexes);
                if (!nm2Buffer.GetBytes(reinterpret_cast<u8*>(skin.vertexIndexes.data()), numSkinVertexIndexes * sizeof(u16)))
                    return false;
            }

            u32 numSkinIndices = 0;
            if (!nm2Buffer.GetU32(numSkinIndices))
                return false;

            if (numSkinIndices > 0)
            {
                skin.indices.resize(numSkinIndices);
                if (!nm2Buffer.GetBytes(reinterpret_cast<u8*>(skin.indices.data()), numSkinIndices * sizeof(u16)))
                    return false;
            }

            u32 numSkinSubMeshes = 0;
            if (!nm2Buffer.GetU32(numSkinSubMeshes))
                return false;

            mesh.subMeshes.resize(numSkinSubMeshes);

            vertexPositions.reserve(numSkinVertexIndexes);
            uvCoordinates.reserve(numSkinVertexIndexes);
            for (const u16& vertexIndex : skin.vertexIndexes)
            {
                const NM2::M2Vertex& vertex = nm2.vertices[vertexIndex];

                vertexPositions.push_back(vertex.position);
                uvCoordinates.push_back(vertex.uvCords[0]);
            }

            for (u32 j = 0; j < numSkinSubMeshes; j++)
            {
                SubMesh& subMesh = mesh.subMeshes[j];

                if (!nm2Buffer.GetU16(subMesh.indexStart))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.indexCount))
                    return false;

                if (!nm2Buffer.GetU8(subMesh.flags))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.shaderId))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.skinSectionIndex))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.geosetIndex))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.materialIndex))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.textureCount))
                    return false;

                if (!nm2Buffer.GetU16(subMesh.textureComboIndex))
                    return false;

                subMesh.materialNum = static_cast<u32>(materials.size());

                Material& material = materials.emplace_back();
                material.flags = subMesh.flags;

                u16 offset = subMesh.textureComboIndex;
                for (u16 x = subMesh.textureComboIndex; x < subMesh.textureComboIndex + subMesh.textureCount; x++)
                {
                    // Set TextureId to the offset into nm2.textures (Below we will load all textures and then we will resolve the actual texture id)
                    material.textureIDs[x - offset] = nm2.textureCombos[x];
                }

                // -- Create Index Buffer --
                {
                    const size_t bufferSize = subMesh.indexCount * sizeof(u16);

                    Renderer::BufferDesc desc;
                    desc.name = "IndexBuffer";
                    desc.size = bufferSize;
                    desc.usage = Renderer::BufferUsage::BUFFER_USAGE_INDEX_BUFFER | Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_DESTINATION;
                    desc.cpuAccess = Renderer::BufferCPUAccess::None;

                    subMesh.indexBuffer = _renderer->CreateBuffer(desc);

                    // Create staging buffer
                    desc.name = "IndexBufferStaging";
                    desc.size = bufferSize;
                    desc.usage = Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_SOURCE;
                    desc.cpuAccess = Renderer::BufferCPUAccess::WriteOnly;

                    Renderer::BufferID stagingBuffer = _renderer->CreateBuffer(desc);

                    // Upload to staging buffer

                    void* dst = _renderer->MapBuffer(stagingBuffer);
                    memcpy(dst, &skin.indices.data()[subMesh.indexStart], bufferSize);
                    _renderer->UnmapBuffer(stagingBuffer);

                    // Queue destroy staging buffer
                    _renderer->QueueDestroyBuffer(stagingBuffer);

                    // Copy from staging buffer to buffer
                    _renderer->CopyBuffer(subMesh.indexBuffer, 0, stagingBuffer, 0, bufferSize);
                }
            }

            // -- Create Vertex Buffer --
            {
                const size_t bufferSize = vertexPositions.size() * sizeof(vec3);

                // Create buffer
                Renderer::BufferDesc desc;
                desc.name = "VertexPositions";
                desc.size = bufferSize;
                desc.usage = Renderer::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER | Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_DESTINATION;
                desc.cpuAccess = Renderer::BufferCPUAccess::None;

                mesh.vertexPositionsBuffer = _renderer->CreateBuffer(desc);

                // Create staging buffer
                desc.name = "VertexPositionsStaging";
                desc.size = bufferSize;
                desc.usage = Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_SOURCE;
                desc.cpuAccess = Renderer::BufferCPUAccess::WriteOnly;

                Renderer::BufferID stagingBuffer = _renderer->CreateBuffer(desc);

                // Upload to staging buffer
                void* dst = _renderer->MapBuffer(stagingBuffer);

                memcpy(dst, vertexPositions.data(), bufferSize);
                _renderer->UnmapBuffer(stagingBuffer);

                // Queue destroy staging buffer
                _renderer->QueueDestroyBuffer(stagingBuffer);

                // Copy from staging buffer to buffer
                _renderer->CopyBuffer(mesh.vertexPositionsBuffer, 0, stagingBuffer, 0, bufferSize);
            }
            
            // -- Create UV Buffer --
            {
                const size_t bufferSize = uvCoordinates.size() *sizeof(vec2);

                // Create buffer
                Renderer::BufferDesc desc;
                desc.name = "VertexUVs";
                desc.size = bufferSize;
                desc.usage = Renderer::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER | Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_DESTINATION;
                desc.cpuAccess = Renderer::BufferCPUAccess::None;

                mesh.vertexUVsBuffer = _renderer->CreateBuffer(desc);

                // Create staging buffer
                desc.name = "VertexUVsStaging";
                desc.size = bufferSize;
                desc.usage = Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_SOURCE;
                desc.cpuAccess = Renderer::BufferCPUAccess::WriteOnly;

                Renderer::BufferID stagingBuffer = _renderer->CreateBuffer(desc);

                // Upload to staging buffer
                void* dst = _renderer->MapBuffer(stagingBuffer);

                memcpy(dst, uvCoordinates.data(), bufferSize);
                _renderer->UnmapBuffer(stagingBuffer);

                // Queue destroy staging buffer
                _renderer->QueueDestroyBuffer(stagingBuffer);

                // Copy from staging buffer to buffer
                _renderer->CopyBuffer(mesh.vertexUVsBuffer, 0, stagingBuffer, 0, bufferSize);
            }
        }
    }

    entt::registry* registry = ServiceLocator::GetGameRegistry();
    TextureSingleton& textureSingleton = registry->ctx<TextureSingleton>();

    // Load all textures into TextureArray
    for (u32 i = 0; i < numTextures; i++)
    {
        const NM2::M2Texture& texture = nm2.textures[i];
        u32& textureId = loadedNM2.textureIds.emplace_back();

        if (texture.type == 0 && texture.textureNameIndex != std::numeric_limits<u32>().max())
        {
            Renderer::TextureDesc textureDesc;
            textureDesc.path = "Data/extracted/Textures/" + textureSingleton.textureStringTable.GetString(texture.textureNameIndex);

            _renderer->LoadTextureIntoArray(textureDesc, _m2Textures, textureId);
        }
        else
        {
            textureId = INVALID_M2_TEXTURE_ID;
        }
    }

    // Resolve all material texture ids
    for (Material& material : materials)
    {
        for (i32 i = 0; i < 4; i++)
        {
            u32& textureId = material.textureIDs[i];
            if (textureId == INVALID_M2_TEXTURE_ID)
                continue;

            textureId = loadedNM2.textureIds[textureId];
        }
    }

    // -- Create Materials Buffer
    {
        const size_t oneBufferSize = sizeof(Material);
        const size_t totalBufferSize = materials.size() * oneBufferSize;

        // Create buffer
        Renderer::BufferDesc desc;
        desc.name = "Materials";
        desc.size = totalBufferSize;
        desc.usage = Renderer::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER | Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_DESTINATION;
        desc.cpuAccess = Renderer::BufferCPUAccess::None;

        loadedNM2.materialsBuffer = _renderer->CreateBuffer(desc);

        // Create staging buffer
        desc.name = "MaterialsStaging";
        desc.size = totalBufferSize;
        desc.usage = Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_SOURCE;
        desc.cpuAccess = Renderer::BufferCPUAccess::WriteOnly;

        Renderer::BufferID stagingBuffer = _renderer->CreateBuffer(desc);

        // Upload to staging buffer
        void* dst = _renderer->MapBuffer(stagingBuffer);

        for (i32 i = 0; i < materials.size(); i++)
        {
            Material& material = materials[i];

            size_t offset = i * oneBufferSize;
            memcpy(static_cast<u8*>(dst) + offset, &material, oneBufferSize);
        }

        _renderer->UnmapBuffer(stagingBuffer);

        // Queue destroy staging buffer
        _renderer->QueueDestroyBuffer(stagingBuffer);

        // Copy from staging buffer to buffer
        _renderer->CopyBuffer(loadedNM2.materialsBuffer, 0, stagingBuffer, 0, totalBufferSize);
    }

    // -- Create Instance Buffer
    {
        const size_t bufferSize = MAX_INSTANCES * sizeof(Instance);

        // Create buffer
        Renderer::BufferDesc desc;
        desc.name = "Instance";
        desc.size = bufferSize;
        desc.usage = Renderer::BufferUsage::BUFFER_USAGE_STORAGE_BUFFER | Renderer::BufferUsage::BUFFER_USAGE_TRANSFER_DESTINATION;
        desc.cpuAccess = Renderer::BufferCPUAccess::None;

        loadedNM2.instanceBuffer = _renderer->CreateBuffer(desc);
    }

    objectID = nextID;
    return true;
}
