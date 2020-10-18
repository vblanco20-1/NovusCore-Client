#pragma once
#include <NovusTypes.h>
#include <Renderer/Renderer.h>
#include <Renderer/Buffer.h>

namespace UI
{
#pragma pack(push, 1)
    struct Box
    {
        u32 top = 0;
        u32 right = 0;
        u32 bottom = 0;
        u32 left = 0;
    };
#pragma pack(pop)


    struct ImageStylesheet
    {
        std::string texture = "";
        Color color = Color(1, 1, 1, 1);
        Box slicingOffset;
    };
}

namespace UIComponent
{
    struct Image
    {
    public:
        struct ImageConstantBuffer
        {
            Color color; // 16 bytes
            UI::Box slicingOffset; // 16 bytes
            vec2 size ; // 8 bytes

            u8 padding[216] = {};
        };
        Image(){ }

        UI::ImageStylesheet style;
        Renderer::TextureID textureID = Renderer::TextureID::Invalid();
        Renderer::BufferID vertexBufferID = Renderer::BufferID::Invalid();
        Renderer::Buffer<ImageConstantBuffer>* constantBuffer = nullptr;
    };
}