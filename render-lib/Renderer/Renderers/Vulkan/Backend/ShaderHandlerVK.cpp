#include "ShaderHandlerVK.h"
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include "RenderDeviceVK.h"
#include <fstream>

namespace Renderer
{
    namespace Backend
    {
        ShaderHandlerVK::ShaderHandlerVK()
        {

        }

        ShaderHandlerVK::~ShaderHandlerVK()
        {
            for (Shader& shader : _vertexShaders)
            {
                vkDestroyShaderModule(shader.device->_device, shader.module, nullptr);
            }
            for (Shader& shader : _pixelShaders)
            {
                vkDestroyShaderModule(shader.device->_device, shader.module, nullptr);
            }
            for (Shader& shader : _computeShaders)
            {
                vkDestroyShaderModule(shader.device->_device, shader.module, nullptr);
            }
            _vertexShaders.clear();
            _pixelShaders.clear();
            _computeShaders.clear();
        }

        VertexShaderID ShaderHandlerVK::LoadShader(RenderDeviceVK* device, const VertexShaderDesc& desc)
        {
            return LoadShader<VertexShaderID>(device, desc.path, _vertexShaders);
        }

        PixelShaderID ShaderHandlerVK::LoadShader(RenderDeviceVK* device, const PixelShaderDesc& desc)
        {
            return LoadShader<PixelShaderID>(device, desc.path, _pixelShaders);
        }

        ComputeShaderID ShaderHandlerVK::LoadShader(RenderDeviceVK* device, const ComputeShaderDesc& desc)
        {
            return LoadShader<ComputeShaderID>(device, desc.path, _computeShaders);
        }

        void ShaderHandlerVK::ReadFile(const std::string& filename, ShaderBinary& binary)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                NC_LOG_FATAL("Failed to open file!");
            }

            size_t fileSize = (size_t)file.tellg();
            binary.resize(fileSize);

            file.seekg(0);
            file.read(binary.data(), fileSize);

            file.close();
        }

        VkShaderModule ShaderHandlerVK::CreateShaderModule(RenderDeviceVK* device, const ShaderBinary& binary)
        {
            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = binary.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(binary.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device->_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create shader module!");
            }

            return shaderModule;
        }

        bool ShaderHandlerVK::TryFindExistingShader(const std::string& shaderPath, std::vector<Shader>& shaders, size_t& id)
        {
            u32 shaderPathHash = StringUtils::fnv1a_32(shaderPath.c_str(), shaderPath.length());

            id = 0;
            for (Shader& existingShader : shaders)
            {
                if (StringUtils::fnv1a_32(existingShader.path.c_str(), existingShader.path.length()) == shaderPathHash)
                {
                    return true;
                }
                id++;
            }

            return false;
        }
    }
}