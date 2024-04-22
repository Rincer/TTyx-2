#include "stdafx.h"

#include "../../Memory/MemoryManager.h"
#include "../../Diagnostics/Debugging.h"
#include "../../Rendering/Renderer.h"
#include "../DeviceState.h"
#include "../../Utilities/Hash.h"
#include "DeviceVK.h"

#include "ShadersVK.h"

ShadersVK::Shader* ShadersVK::m_Shaders[ShaderStageCount];
uint32_t    ShadersVK::m_ShaderCounts[ShaderStageCount];

void ShadersVK::Initialize()
{
    for (uint32_t i = 0; i < ShaderStageCount; i++)
    {
        m_Shaders[i] = static_cast<Shader*>(CMemoryManager::GetAllocator().Alloc(kShaderBlock * sizeof(Shader)));
        m_ShaderCounts[i] = 0;
    }
}

void ShadersVK::Dispose(DeviceVK* pDevice)
{
    for (uint32_t i = 0; i < ShaderStageCount; i++)
    {
        for (uint32_t j = 0; j < m_ShaderCounts[i]; j++)
        {
            Shader& shader = m_Shaders[i][j];
            CMemoryManager::GetAllocator().Free(shader.m_FilePath);
            CMemoryManager::GetAllocator().Free(shader.m_Code);
            pDevice->DestroyShaderModule(shader.m_ShaderModule);
        }
        CMemoryManager::GetAllocator().Free(m_Shaders[i]);
    }
}

VkShaderModule ShadersVK::GetShaderModule(uint32_t index, ShaderStageType stageType)
{
    return m_Shaders[stageType][index].m_ShaderModule;
}

int ShadersVK::Load(DeviceVK* pDevice, const char* filePath, ShaderStageType stageType)
{
    uint64_t hash = Hash::Hash64(reinterpret_cast<const uint8_t*>(filePath), static_cast<uint32_t>(strlen(filePath)));

    // try and find a shader with the same hashed name
    uint32_t i;
    for (i = 0; i < m_ShaderCounts[stageType]; i++)
    {
        if (m_Shaders[stageType][i].m_Hash == hash)
            return i;
    }

    Assert(m_ShaderCounts[stageType] < kShaderBlock);
    Shader& shader = m_Shaders[stageType][m_ShaderCounts[stageType]];
    shader.m_FilePath = static_cast<char*>(CMemoryManager::GetAllocator().Alloc(strlen(filePath) + 1));
    strcpy_s(shader.m_FilePath, strlen(filePath) + 1, filePath);
    shader.m_Hash = hash;
    FILE* f = nullptr;
    fopen_s(&f, filePath, "rb");
    if (f != nullptr)
    {
        shader.m_CodeLength = _filelength(_fileno(f));
        shader.m_Code = static_cast<char*>(CMemoryManager::GetAllocator().Alloc(shader.m_CodeLength));
        fread_s(shader.m_Code, shader.m_CodeLength, 1, shader.m_CodeLength, f);
        fclose(f);
        shader.m_ShaderModule = pDevice->CreateShaderModule(shader.m_Code, shader.m_CodeLength);
        m_ShaderCounts[stageType]++;
        return m_ShaderCounts[stageType] - 1;
    }    
    else
    {
        Assert(0);
    }
    return -1;
}

