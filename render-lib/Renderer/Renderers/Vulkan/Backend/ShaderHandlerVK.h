#pragma once
#include <NovusTypes.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>
#include "../../../Descriptors/VertexShaderDesc.h"
#include "../../../Descriptors/PixelShaderDesc.h"
#include "../../../Descriptors/ComputeShaderDesc.h"

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        typedef std::vector<char> ShaderBinary;

        class ShaderHandlerVK
        {
            using vsIDType = type_safe::underlying_type<VertexShaderID>;
            using psIDType = type_safe::underlying_type<PixelShaderID>;
            using csIDType = type_safe::underlying_type<ComputeShaderID>;

        public:
            ShaderHandlerVK();
            ~ShaderHandlerVK();

            VertexShaderID LoadShader(RenderDeviceVK* device, const VertexShaderDesc& desc);
            PixelShaderID LoadShader(RenderDeviceVK* device, const PixelShaderDesc& desc);
            ComputeShaderID LoadShader(RenderDeviceVK* device, const ComputeShaderDesc& desc);

            VkShaderModule GetShaderModule(const VertexShaderID id) { return _vertexShaders[static_cast<vsIDType>(id)].module; }
            VkShaderModule GetShaderModule(const PixelShaderID id) { return _pixelShaders[static_cast<psIDType>(id)].module; }
            VkShaderModule GetShaderModule(const ComputeShaderID id) { return _computeShaders[static_cast<csIDType>(id)].module; }

            const ShaderBinary* GetSPIRV(const VertexShaderID id) { return &_vertexShaders[static_cast<vsIDType>(id)].spirv; }
            const ShaderBinary* GetSPIRV(const PixelShaderID id) { return &_pixelShaders[static_cast<psIDType>(id)].spirv; }

        private:
            struct Shader
            {
                std::string path;
                VkShaderModule module;
                RenderDeviceVK* device;
                ShaderBinary spirv;
            };

        private:
            template <typename T>
            T LoadShader(RenderDeviceVK* device, const std::string& shaderPath, std::vector<Shader>& shaders)
            {
                size_t id;
                using idType = type_safe::underlying_type<T>;

                // If shader is already loaded, return ID of already loaded version
                if (TryFindExistingShader(shaderPath, shaders, id))
                {
                    return T(static_cast<idType>(id));
                }

                id = shaders.size();
                assert(id < T::MaxValue());

                Shader shader;
                ReadFile(shaderPath, shader.spirv);
                shader.path = shaderPath;
                shader.module = CreateShaderModule(device, shader.spirv);
                shader.device = device;
                
                shaders.push_back(shader);

                return T(static_cast<idType>(id));
            }

            
            void ReadFile(const std::string& filename, ShaderBinary& binary);
            VkShaderModule CreateShaderModule(RenderDeviceVK* device, const ShaderBinary& binary);
            bool TryFindExistingShader(const std::string& shaderPath, std::vector<Shader>& shaders, size_t& id);

        private:
            std::vector<Shader> _vertexShaders;
            std::vector<Shader> _pixelShaders;
            std::vector<Shader> _computeShaders;
        };
    }
}
