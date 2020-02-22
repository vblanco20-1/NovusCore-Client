#pragma once

namespace Renderer
{
    namespace Backend
    {
        struct ConstantBufferBackend
        {
            virtual ~ConstantBufferBackend() {}
            virtual void Apply(u32 frameIndex, void* data, size_t size) = 0;
            virtual void* GetGPUResource(u32 frameIndex) = 0;
        };
    }

    template <typename T>
    struct ConstantBuffer
    {
        T resource;

        constexpr size_t GetSize()
        {
            return sizeof(T);
        }

        void Apply(u32 frameIndex)
        {
            backend->Apply(frameIndex, &resource, GetSize());
        }

        void* GetGPUResource(u32 frameIndex)
        {
            return backend->GetGPUResource(frameIndex);
        }

        Backend::ConstantBufferBackend* backend = nullptr;

    protected:
        ConstantBuffer() {}; // This has to be created through Renderer::CreateConstantBuffer<T>

        friend class Renderer;
    };
}