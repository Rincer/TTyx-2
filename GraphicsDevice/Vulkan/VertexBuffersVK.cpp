#include "stdafx.h"
#include "../../Memory/MemoryManager.h"
#include "../../Diagnostics/Debugging.h"
#include "DeviceVK.h"

#include "VertexBuffersVK.h"

void VertexBuffersVK::Initialize()
{
    m_VertexBuffersCount = 0;
    m_pVertexBuffers = static_cast<VertexBuffer*>(CMemoryManager::GetAllocator().Alloc(scm_VertexBuffersBlock * sizeof(VertexBuffer)));
}

void VertexBuffersVK::Dispose(DeviceVK* pGraphicsDevice)
{
    for (uint32_t i = 0; i < m_VertexBuffersCount; i++)
    {
        pGraphicsDevice->DestroyVertexBuffer(&m_pVertexBuffers[i]);
    }
    CMemoryManager::GetAllocator().Free(m_pVertexBuffers);
}

uint32_t VertexBuffersVK::CreateVertexBuffer(DeviceVK* pGraphicsDevice, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t size, const void* pData, uint32_t numVertices)
{
    Assert(m_VertexBuffersCount < scm_VertexBuffersBlock);
    m_pVertexBuffers[m_VertexBuffersCount].m_InputState = vertexInputState;

    VertexInputState& vis = m_pVertexBuffers[m_VertexBuffersCount].m_InputState;
    vis.m_VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vis.m_VertexInput.vertexBindingDescriptionCount = vis.m_BindingsCount;
    vis.m_VertexInput.pVertexBindingDescriptions = vis.m_Bindings; 
    vis.m_VertexInput.vertexAttributeDescriptionCount = vis.m_AttributesCount;
    vis.m_VertexInput.pVertexAttributeDescriptions = vis.m_Attributes;
    vis.m_NumVerts = numVertices;
    pGraphicsDevice->CreateVertexBuffer(m_pVertexBuffers[m_VertexBuffersCount].m_VertexBuffer, m_pVertexBuffers[m_VertexBuffersCount].m_BufferMemory, size, pData);

    m_VertexBuffersCount++;
    return m_VertexBuffersCount - 1;
}

const VkPipelineVertexInputStateCreateInfo& VertexBuffersVK::GetVertexInputCreateInfo(uint32_t vertexBufferIndex) const
{
    return m_pVertexBuffers[vertexBufferIndex].m_InputState.GetVertexInput();
}

void VertexBuffersVK::GetVertexBuffer(VkBuffer& buffer, uint32_t& numVertices, uint32_t vertexBufferIndex) const
{
    buffer = m_pVertexBuffers[vertexBufferIndex].m_VertexBuffer;
    numVertices = m_pVertexBuffers[vertexBufferIndex].m_InputState.m_NumVerts;
}

VertexBuffersVK::VertexInputState::VertexInputState()
{
    m_BindingsCount = 0;
    m_AttributesCount = 0;
    memset(&m_VertexInput, 0, sizeof(m_VertexInput));
}

void VertexBuffersVK::VertexInputState::AddBinding(uint32_t slot, uint32_t stride)
{
    Assert(m_BindingsCount < scm_MaxBindings);
    m_Bindings[m_BindingsCount].binding = slot;
    m_Bindings[m_BindingsCount].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    m_Bindings[m_BindingsCount].stride = stride;
    m_BindingsCount++;
}

void VertexBuffersVK::VertexInputState::AddAttribute(uint32_t slot, uint32_t location, VkFormat format, uint32_t offset)
{
    Assert(m_AttributesCount < scm_MaxDescriptors);
    m_Attributes[m_AttributesCount].binding = slot;
    m_Attributes[m_AttributesCount].location = location;
    m_Attributes[m_AttributesCount].format = format;
    m_Attributes[m_AttributesCount].offset = offset;
    m_AttributesCount++;
}

const VkPipelineVertexInputStateCreateInfo& VertexBuffersVK::VertexInputState::GetVertexInput()
{
    m_VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_VertexInput.vertexBindingDescriptionCount = m_BindingsCount;
    m_VertexInput.vertexAttributeDescriptionCount = m_AttributesCount;
    m_VertexInput.pVertexBindingDescriptions = m_Bindings;
    m_VertexInput.pVertexAttributeDescriptions = m_Attributes;
    return m_VertexInput;
}
