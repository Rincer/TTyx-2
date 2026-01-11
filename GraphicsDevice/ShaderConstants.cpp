#include "stdafx.h"
#include "../Diagnostics/Debugging.h"
#include "../Utilities/Hash.h"
#include "Vulkan\DeviceVK.h"

#include "ShaderConstants.h"

void ShaderConstants::Initialize()
{
    m_NumBuffers = 0;
}

void ShaderConstants::Dispose(DeviceVK* pDevice)
{
    for (uint32_t i = 0; i < m_NumBuffers; i++)
    {
        BufferInfo& cb = m_BufferInfos[i];
        pDevice->DestroyBuffer(cb.m_buffer, cb.m_BufferMemory);
    }
}

void ShaderConstants::AddConstantBuffers(DeviceVK* pDevice, const SpvReflectShaderModule& module)
{
    if (module.descriptor_set_count == 0)
        return;
    Assert(module.descriptor_set_count == 1);
    const SpvReflectDescriptorSet& descriptorSet = module.descriptor_sets[0];
    for (uint32_t i = 0; i < descriptorSet.binding_count; i++)
    {
        SpvReflectDescriptorBinding* pDescriptorBinding = descriptorSet.bindings[i];
        switch (pDescriptorBinding->descriptor_type)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        {
            for (uint32_t j = 0; j < m_NumBuffers; j++)
            {
                // Check if buffer already exists
                if (!strcmp(pDescriptorBinding->block.name, m_BufferInfos[i].m_Name))
                    continue;
            }
            Assert(m_NumBuffers < scm_ConstantBufferBlock);
            BufferInfo& bi = m_BufferInfos[m_NumBuffers];
            bi.m_Size = pDescriptorBinding->block.size;
            strcpy_s(bi.m_Name, scm_MaxNameLength - 1, pDescriptorBinding->block.name);
            pDevice->CreateConstantBuffer(bi.m_buffer, bi.m_BufferMemory, bi.m_Size);

            ConstantBuffer& cb = m_ConstantBuffers[m_NumBuffers];
            cb.m_NumConstants = pDescriptorBinding->block.member_count;
            for (uint32_t i = 0; i < pDescriptorBinding->block.member_count; i++)
            {
                Constant& c = cb.m_Constants[i];
                strcpy_s(c.m_Name, scm_MaxNameLength - 1, pDescriptorBinding->block.members[i].name);
                c.m_Offset = pDescriptorBinding->block.members[i].offset;
                c.m_Size = pDescriptorBinding->block.members[i].size;
            }
            m_NumBuffers++;
            break;
        }
        default:
            Assert(0);
            break;
        }
    }
}

void ShaderConstants::UpdateConstant(DeviceVK* pDevice, const char* pBufferName, const char* pConstantName, void* pData, uint32_t size)
{
    uint32_t bufferIndex = 0;
    for (; bufferIndex < m_NumBuffers; bufferIndex++)
    {
        const BufferInfo& bi = m_BufferInfos[bufferIndex];
        if (!strcmp(bi.m_Name, pBufferName))
        {
            break;
        }
    }
    Assert(bufferIndex < m_NumBuffers);
    const ConstantBuffer& cb = m_ConstantBuffers[bufferIndex];
    uint32_t constantIndex = 0;
    for (; constantIndex < cb.m_NumConstants; constantIndex++)
    {
        const Constant& c = cb.m_Constants[constantIndex];
        if (!strcmp(c.m_Name, pConstantName))
        {
            break;
        }
    }
    Assert(constantIndex < cb.m_NumConstants);
    const Constant& c = cb.m_Constants[constantIndex];
    Assert(size <= c.m_Size);
    const BufferInfo& bi = m_BufferInfos[bufferIndex];
    pDevice->UpdateBuffer(bi.m_buffer, c.m_Offset, c.m_Size, pData);
}

void ShaderConstants::GetBufferInfos(const BufferInfo*& bufferInfos, uint32_t& numBuffers) const
{
    bufferInfos = m_BufferInfos;
    numBuffers = m_NumBuffers;
}