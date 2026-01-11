#include "stdafx.h"

#include "../../Memory/MemoryManager.h"
#include "../../Diagnostics/Debugging.h"
#include "../../Rendering/Renderer.h"
#include "../DeviceState.h"
#include "../../Utilities/Hash.h"
#include "DeviceVK.h"

#include "ShadersVK.h"


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
            CMemoryManager::GetAllocator().Free(const_cast<char*>(shader.m_Code));
            pDevice->DestroyShaderModule(shader.m_ShaderModule);
            if (shader.m_DescriptorSetLayoutValid)
            {
                pDevice->DestroyDescriptorSetLayout(shader.m_DescriptorSetLayout);
            }
        }
        CMemoryManager::GetAllocator().Free(m_Shaders[i]);
    }
}

VkShaderModule ShadersVK::GetShaderModule(uint32_t index, ShaderStageType stageType) const
{
    return m_Shaders[stageType][index].m_ShaderModule;
}

bool ShadersVK::IsDescriptorSetLayoutValid(uint32_t index, ShaderStageType stageType) const
{
    return m_Shaders[stageType][index].m_DescriptorSetLayoutValid;
}

VkDescriptorSetLayout ShadersVK::GetDescriptorSetLayout(uint32_t index, ShaderStageType stageType) const
{
    return m_Shaders[stageType][index].m_DescriptorSetLayout;
}

VkDescriptorSet ShadersVK::GetDescriptorSet(uint32_t index, ShaderStageType stageType) const
{
    return m_Shaders[stageType][index].m_DescriptorSet;
}

int ShadersVK::GetIndex(uint64_t hash, ShaderStageType stageType) const
{
    // try and find a shader with the same hashed name
    uint32_t i;
    for (i = 0; i < m_ShaderCounts[stageType]; i++)
    {
        if (m_Shaders[stageType][i].m_Hash == hash)
            return i;
    }
    return -1;
}

int ShadersVK::Load(DeviceVK* pDevice, const char* code, uint32_t codeLength, uint64_t hash, const ShaderConstants::BufferInfo* bufferInfos, uint32_t bufferCount, const SpvReflectShaderModule& module, ShaderStageType stageType)
{    
    Assert(m_ShaderCounts[stageType] < kShaderBlock);
    Shader& shader = m_Shaders[stageType][m_ShaderCounts[stageType]];
    shader.m_CodeLength = codeLength;
    shader.m_Code = code;
    shader.m_ShaderModule = pDevice->CreateShaderModule(shader.m_Code, shader.m_CodeLength);
    shader.m_DescriptorSetLayoutValid = pDevice->CreateDescriptorSetLayout(module, shader.m_DescriptorSetLayout, shader.m_DescriptorSet, bufferInfos, bufferCount);
    m_ShaderCounts[stageType]++;
    return m_ShaderCounts[stageType] - 1;
}

