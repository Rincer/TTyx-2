#ifndef _DEVICESTATE_H_
#define _DEVICESTATE_H_

enum ShaderStageType
{
    ShaderStageVertex = 0,
    ShaderStageFragment,
    ShaderStageCount
};

struct DynamicState
{
    uint32_t m_Width;
    uint32_t m_Height;
};

struct StaticState
{
    // Use Vulkan for now. 
    VkCullModeFlagBits m_CullMode;
    VkFrontFace m_FrontFace;
    uint32_t    m_ColorBlend;
    uint32_t    m_Shaders[ShaderStageCount];
    uint32_t    m_VertexBuffer;
};

class DeviceState
{
    public:
        DeviceState();
        void SetWidth(uint32_t width) { m_DynamicState.m_Width = width; }
        void SetHeight(uint32_t height) { m_DynamicState.m_Height = height; }
        void SetCullMode(VkCullModeFlagBits cullMode); 
        void SetFrontFace(VkFrontFace frontFace); 
        void SetColorBlend(uint32_t colorBlend); 
        void SetShader(uint32_t shader, ShaderStageType shaderType);
        void SetVertexBuffer(uint32_t vertexBuffer);
        void Update();

        uint32_t GetWidth() const { return m_DynamicState.m_Width; }
        uint32_t GetHeight() const { return m_DynamicState.m_Height; }
        VkCullModeFlagBits GetCullMode() const { return m_StaticState.m_CullMode; }
        VkFrontFace GetFrontFace() const { return m_StaticState.m_FrontFace; }
        uint32_t GetColorBlend() const { return m_StaticState.m_ColorBlend; }
        uint32_t GetShader(ShaderStageType shaderType) const { return m_StaticState.m_Shaders[shaderType]; }       
        uint64_t GetHash() const { return m_Hash; }

    private:
        DynamicState    m_DynamicState;
        StaticState     m_StaticState;
        bool            m_IsChanged;
        uint64_t        m_Hash;
};

#endif
