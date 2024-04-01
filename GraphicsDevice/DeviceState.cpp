#include "stdafx.h"

#include "DeviceState.h"
#include "..\Utilities\Hash.h"


DeviceState::DeviceState()
{
    Update();
}

void DeviceState::SetCullMode(VkCullModeFlagBits cullMode) 
{ 
    if (m_StaticState.m_CullMode != cullMode)
    {
        m_StaticState.m_CullMode = cullMode;
        m_IsChanged = true;
    }
}

void DeviceState::SetFrontFace(VkFrontFace frontFace) 
{ 
    if (m_StaticState.m_FrontFace != frontFace)
    {
        m_StaticState.m_FrontFace = frontFace;
        m_IsChanged = true;
    }         
}

void DeviceState::SetColorBlend(uint32_t colorBlend) 
{ 
    if (m_StaticState.m_ColorBlend != colorBlend)
    {
        m_StaticState.m_ColorBlend = colorBlend;
        m_IsChanged = true;
    }
}

void DeviceState::SetShader(uint32_t shader, ShaderStageType shaderType) 
{ 
    if (m_StaticState.m_Shaders[shaderType] != shader)
    {
        m_StaticState.m_Shaders[shaderType] = shader;
        m_IsChanged = true;
    }
}

void DeviceState::SetVertexBuffer(uint32_t vertexBuffer)
{
    if (m_StaticState.m_VertexBuffer != vertexBuffer)
    {
        m_StaticState.m_VertexBuffer = vertexBuffer;
        m_IsChanged = true;
    }
}

void DeviceState::Update()
{
    if (m_IsChanged)
    {
        m_Hash = Hash::Hash64(reinterpret_cast<uint8_t*>(&(m_StaticState)), sizeof(StaticState));
        m_IsChanged = false;
    }
}
