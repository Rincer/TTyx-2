#include "stdafx.h"
#include "../../Memory/MemoryManager.h"
#include "../../Diagnostics/Debugging.h"
#include "DeviceVK.h"

#include "IndexBuffersVK.h"

void IndexBuffersVK::Initialize()
{
    m_IndexBuffersCount = 0;
    m_pIndexBuffers = static_cast<IndexBuffer*>(CMemoryManager::GetAllocator().Alloc(scm_IndexBuffersBlock * sizeof(IndexBuffer)));
}

void IndexBuffersVK::Dispose(DeviceVK* pGraphicsDevice)
{
    for (uint32_t i = 0; i < m_IndexBuffersCount; i++)
    {
        pGraphicsDevice->DestroyIndexBuffer(&m_pIndexBuffers[i]);
    }
    CMemoryManager::GetAllocator().Free(m_pIndexBuffers);
}

uint32_t IndexBuffersVK::CreateIndexBuffer(DeviceVK* pGraphicsDevice, uint32_t size, const void* pData, uint32_t numIndices)
{
    Assert(m_IndexBuffersCount < scm_IndexBuffersBlock);
    m_pIndexBuffers[m_IndexBuffersCount].m_NumIndices = numIndices;
    pGraphicsDevice->CreateIndexBuffer(m_pIndexBuffers[m_IndexBuffersCount].m_IndexBuffer, m_pIndexBuffers[m_IndexBuffersCount].m_BufferMemory, size, pData);
    m_IndexBuffersCount++;
    return m_IndexBuffersCount - 1;
}

void IndexBuffersVK::GetIndexBuffer(VkBuffer& buffer, uint32_t& numIndices, uint32_t indexBufferIndex) const
{
    buffer = m_pIndexBuffers[indexBufferIndex].m_IndexBuffer;
    numIndices = m_pIndexBuffers[indexBufferIndex].m_NumIndices;
}