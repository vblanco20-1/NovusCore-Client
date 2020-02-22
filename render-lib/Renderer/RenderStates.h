#pragma once
#include <NovusTypes.h>
#include <cassert>

namespace Renderer
{
    static const int MAX_RENDER_TARGETS = 8;

    enum FillMode
    {
        FILL_MODE_SOLID,
        FILL_MODE_WIREFRAME
    };

    enum CullMode
    {
        CULL_MODE_NONE,
        CULL_MODE_FRONT,
        CULL_MODE_BACK
    };

    enum SampleCount
    {
        SAMPLE_COUNT_1,
        SAMPLE_COUNT_2,
        SAMPLE_COUNT_4,
        SAMPLE_COUNT_8
    };

    constexpr int SampleCountToInt(SampleCount sampleCount)
    {
        switch (sampleCount)
        {
            case SAMPLE_COUNT_1: return 1;
            case SAMPLE_COUNT_2: return 2;
            case SAMPLE_COUNT_4: return 4;
            case SAMPLE_COUNT_8: return 8;
            default:
                assert(false); // Invalid sample count, did we just add to the enum?
        }
        return 0;
    }

    enum FrontFaceState
    {
        FRONT_FACE_STATE_CLOCKWISE,
        FRONT_FACE_STATE_COUNTERCLOCKWISE
    };

    struct RasterizerState
    {
        FillMode fillMode = FILL_MODE_SOLID;
        CullMode cullMode = CULL_MODE_BACK;
        FrontFaceState frontFaceMode = FRONT_FACE_STATE_CLOCKWISE;
        bool depthBiasEnabled = false;
        i32 depthBias = 0;
        f32 depthBiasClamp = 0.0f;
        f32 depthBiasSlopeFactor = 0.0f;
        SampleCount sampleCount = SAMPLE_COUNT_1;
    };

    enum BlendMode
    {
        BLEND_MODE_ZERO,
        BLEND_MODE_ONE,
        BLEND_MODE_SRC_COLOR,
        BLEND_MODE_INV_SRC_COLOR,
        BLEND_MODE_SRC_ALPHA,
        BLEND_MODE_INV_SRC_ALPHA,
        BLEND_MODE_DEST_ALPHA,
        BLEND_MODE_INV_DEST_ALPHA,
        BLEND_MODE_DEST_COLOR,
        BLEND_MODE_INV_DEST_COLOR,
        BLEND_MODE_SRC_ALPHA_SAT,
        BLEND_MODE_BLEND_FACTOR,
        BLEND_MODE_INV_BLEND_FACTOR,
        BLEND_MODE_SRC1_COLOR,
        BLEND_MODE_INV_SRC1_COLOR,
        BLEND_MODE_SRC1_ALPHA,
        BLEND_MODE_INV_SRC1_ALPHA
    };

    enum BlendOp
    {
        BLEND_OP_ADD,
        BLEND_OP_SUBTRACT,
        BLEND_OP_REV_SUBTRACT,
        BLEND_OP_MIN,
        BLEND_OP_MAX
    };

    enum LogicOp
    {
        LOGIC_OP_CLEAR,
        LOGIC_OP_SET,
        LOGIC_OP_COPY,
        LOGIC_OP_COPY_INVERTED,
        LOGIC_OP_NOOP,
        LOGIC_OP_INVERT,
        LOGIC_OP_AND,
        LOGIC_OP_NAND,
        LOGIC_OP_OR,
        LOGIC_OP_NOR,
        LOGIC_OP_XOR,
        LOGIC_OP_EQUIV,
        LOGIC_OP_AND_REVERSE,
        LOGIC_OP_AND_INVERTED,
        LOGIC_OP_OR_REVERSE,
        LOGIC_OP_OR_INVERTED
    };

    enum ColorWriteEnable
    {
        COLOR_WRITE_ENABLE_RED = 1,
        COLOR_WRITE_ENABLE_GREEN = 2,
        COLOR_WRITE_ENABLE_BLUE = 4,
        COLOR_WRITE_ENABLE_ALPHA = 8,
        COLOR_WRITE_ENABLE_ALL = (COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN | COLOR_WRITE_ENABLE_BLUE | COLOR_WRITE_ENABLE_ALPHA)
    };

    struct RTBlendState
    {
        bool blendEnable = false;
        bool logicOpEnable = false;

        BlendMode srcBlend = BLEND_MODE_ONE;
        BlendMode destBlend = BLEND_MODE_ZERO;
        BlendOp blendOp = BLEND_OP_ADD;

        BlendMode srcBlendAlpha = BLEND_MODE_ONE;
        BlendMode destBlendAlpha = BLEND_MODE_ZERO;
        BlendOp blendOpAlpha = BLEND_OP_ADD;

        LogicOp logicOp = LOGIC_OP_NOOP;
        u8 renderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
    };

    struct BlendState
    {
        bool alphaToCoverageEnable = false;
        bool independencBlendEnable = false;
        RTBlendState renderTargets[MAX_RENDER_TARGETS];
    };

    enum ComparisonFunc
    {
        COMPARISON_FUNC_NEVER,
        COMPARISON_FUNC_LESS,
        COMPARISON_FUNC_EQUAL,
        COMPARISON_FUNC_LESS_EQUAL,
        COMPARISON_FUNC_GREATER,
        COMPARISON_FUNC_NOT_EQUAL,
        COMPARISON_FUNC_GREATER_EQUAL,
        COMPARISON_FUNC_ALWAYS
    };

    enum StencilOp
    {
        STENCIL_OP_KEEP,
        STENCIL_OP_ZERO,
        STENCIL_OP_REPLACE,
        STENCIL_OP_INCR_SAT,
        STENCIL_OP_DECR_SAT,
        STENCIL_OP_INVERT,
        STENCIL_OP_INCR,
        STENCIL_OP_DECR
    };

    struct DepthStencilOpDesc
    {
        StencilOp stencilFailOp = STENCIL_OP_KEEP;
        StencilOp stencilDepthFailOp = STENCIL_OP_KEEP;
        StencilOp stencilPassOp = STENCIL_OP_KEEP;
        ComparisonFunc stencilFunc = COMPARISON_FUNC_ALWAYS;
    };

    struct DepthStencilState
    {
        bool depthEnable = false;
        bool depthWriteEnable = false;
        ComparisonFunc depthFunc = COMPARISON_FUNC_LESS;
        bool stencilEnable = false;
        u8 stencilReadMask = 255;
        u8 stencilWriteMask = 255;
        DepthStencilOpDesc frontFace;
        DepthStencilOpDesc backFace;
    };

    enum ShaderVisibility
    {
        SHADER_VISIBILITY_ALL,
        SHADER_VISIBILITY_VERTEX,
        SHADER_VISIBILITY_HULL,
        SHADER_VISIBILITY_DOMAIN,
        SHADER_VISIBILITY_GEOMETRY,
        SHADER_VISIBILITY_PIXEL
    };

    struct ConstantBufferState
    {
        bool enabled = false;
        ShaderVisibility shaderVisibility = SHADER_VISIBILITY_ALL;
    };

    enum InputFormat
    {
        INPUT_FORMAT_UNKNOWN,
        // 32 bit per component
        INPUT_FORMAT_R32G32B32A32_FLOAT,
        INPUT_FORMAT_R32G32B32A32_UINT,
        INPUT_FORMAT_R32G32B32A32_SINT,
        INPUT_FORMAT_R32G32B32_FLOAT,
        INPUT_FORMAT_R32G32B32_UINT,
        INPUT_FORMAT_R32G32B32_SINT,
        INPUT_FORMAT_R32G32_FLOAT,
        INPUT_FORMAT_R32G32_UINT,
        INPUT_FORMAT_R32G32_SINT,
        INPUT_FORMAT_R32_FLOAT,
        INPUT_FORMAT_R32_UINT,
        INPUT_FORMAT_R32_SINT,
        // 16 bit per component
        INPUT_FORMAT_R16G16B16A16_FLOAT,
        INPUT_FORMAT_R16G16B16A16_UINT,
        INPUT_FORMAT_R16G16B16A16_SINT,
        INPUT_FORMAT_R16G16_FLOAT,
        INPUT_FORMAT_R16G16_UINT,
        INPUT_FORMAT_R16G16_SINT,
        INPUT_FORMAT_R16_FLOAT,
        INPUT_FORMAT_R16_UINT,
        INPUT_FORMAT_R16_SINT,
        // 8 bit per component
        INPUT_FORMAT_R8G8B8A8_UINT,
        INPUT_FORMAT_R8G8B8A8_SINT,
        INPUT_FORMAT_R8G8_UINT,
        INPUT_FORMAT_R8G8_SINT,
        INPUT_FORMAT_R8_UINT,
        INPUT_FORMAT_R8_SINT,

    };

    enum InputClassification
    {
        INPUT_CLASSIFICATION_PER_VERTEX,
        INPUT_CLASSIFICATION_PER_INSTANCE
    };

    constexpr int INPUT_LAYOUT_NAME_MAX_LENGTH = 16;
    struct InputLayout
    {
        bool enabled = false;
        u32 index = 0;
        InputFormat format = INPUT_FORMAT_UNKNOWN;
        u32 slot = 0;
        u32 alignedByteOffset = 0;
        InputClassification inputClassification = INPUT_CLASSIFICATION_PER_VERTEX;
        u32 instanceDataStepRate = 0;

        const char* GetName() const
        {
            return _name;
        }
        void SetName(const std::string& name)
        {
            assert(name.length() < INPUT_LAYOUT_NAME_MAX_LENGTH); // The name you tried to set on the input layout is too long, either make it shorter or increase INPUT_LAYOUT_NAME_MAX_LENGTH
            memset(_name, 0, INPUT_LAYOUT_NAME_MAX_LENGTH);
            strcpy_s(_name, name.data());
        }

    private:
        char _name[INPUT_LAYOUT_NAME_MAX_LENGTH] = {};
    };

    enum DepthClearFlags
    {
        DEPTH_CLEAR_DEPTH,
        DEPTH_CLEAR_STENCIL,
        DEPTH_CLEAR_BOTH
    };

    struct Viewport
    {
        f32 topLeftX = 0;
        f32 topLeftY = 0;
        f32 width = 0;
        f32 height = 0;
        f32 minDepth = 0;
        f32 maxDepth = 0;
    };

    struct ScissorRect
    {
        i32 left;
        i32 top;
        i32 right;
        i32 bottom;
    };
}