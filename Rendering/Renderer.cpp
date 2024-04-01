#include "stdafx.h"
#include "..\Memory\MemoryManager.h"
#include "..\GraphicsDevice\Vulkan\ShadersVK.h"
#include "..\GraphicsDevice\Vulkan\DeviceVK.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"
#include "..\Diagnostics\Debugging.h"
#include "..\Utilities\Hash.h"

#include "Renderer.h"

void Renderer::Initialize()
{
    m_pGraphicsDevice = static_cast<DeviceVK*>(CMemoryManager::GetAllocator().Alloc(sizeof(DeviceVK)));
    m_pGraphicsDevice->Initialize();
    m_pVertexBuffers = static_cast<VertexBuffersVK*>(CMemoryManager::GetAllocator().Alloc(sizeof(VertexBuffersVK)));
    m_pVertexBuffers->Initialize();
    m_RenderElementsCount = 0;
    m_pRenderElements = static_cast<RenderElement*>(CMemoryManager::GetAllocator().Alloc(scm_RenderElementsBlock * sizeof(RenderElement)));
    m_pRenderElementIndices = static_cast<uint32_t*>(CMemoryManager::GetAllocator().Alloc(scm_QueuedElementsBlock * sizeof(uint32_t)));
    m_RenderElementsIndexCount = 0;    
}

void Renderer::Dispose()
{
    m_pGraphicsDevice->WaitTillIdle();
    m_pVertexBuffers->Dispose(m_pGraphicsDevice);
    m_pGraphicsDevice->Dispose();
    CMemoryManager::GetAllocator().Free(m_pGraphicsDevice);
    CMemoryManager::GetAllocator().Free(m_pVertexBuffers);
    CMemoryManager::GetAllocator().Free(m_pRenderElements);
    CMemoryManager::GetAllocator().Free(m_pRenderElementIndices);
}

void Renderer::DrawRenderElements()
{
    m_pGraphicsDevice->GetSurfaceExtent(m_DeviceState);
    m_DeviceState.SetColorBlend(0);
    m_DeviceState.SetCullMode(VK_CULL_MODE_BACK_BIT);
    m_DeviceState.SetFrontFace(VK_FRONT_FACE_CLOCKWISE);
    for (uint32_t i = 0; i < m_RenderElementsIndexCount; i++)
    {
        RenderElement& renderElement = m_pRenderElements[m_pRenderElementIndices[i]];
        m_DeviceState.SetShader(renderElement.m_VertexShader, ShaderStageVertex);
        m_DeviceState.SetShader(renderElement.m_FragmentShader, ShaderStageFragment);
        m_DeviceState.SetVertexBuffer(renderElement.m_VertexBuffer);
        m_DeviceState.Update();
        std::unordered_map<uint64_t,VkPipeline>::const_iterator it = m_PipelineMap.find(m_DeviceState.GetHash());
        VkPipeline vkPipeline{};
        if (it == m_PipelineMap.end())
        {
            m_pGraphicsDevice->CreateGraphicsPipeline(&m_DeviceState, &vkPipeline, m_pVertexBuffers->GetVertexInputCreateInfo(m_DeviceState.GetVertexBuffer()));
            m_PipelineMap.insert(std::make_pair(m_DeviceState.GetHash(), vkPipeline));
        }
        else
        {
            vkPipeline = it->second;
        }
        m_pGraphicsDevice->BindPipeline(vkPipeline);
        VkBuffer buffer;
        uint32_t numVerts;
        m_pVertexBuffers->GetVertexBuffer(buffer, numVerts, m_DeviceState.GetVertexBuffer());
        m_pGraphicsDevice->DrawVertexBuffer(buffer, 0, numVerts);
    }
}

void Renderer::Execute()
{
    uint32_t imageIndex = m_pGraphicsDevice->BeginFrame();
    if (imageIndex != -1)
    {
        DrawRenderElements();
        m_pGraphicsDevice->EndFrame(imageIndex);        
    }    
    m_RenderElementsIndexCount = 0;
}

uint32_t Renderer::CreateRenderElement(const char* vertexShader, const char* fragmentShader, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t size, const void* pVertexData, uint32_t numVertices)
{
    Assert(m_RenderElementsCount < scm_RenderElementsBlock);
    m_pRenderElements[m_RenderElementsCount].m_VertexShader = ShadersVK::Load(m_pGraphicsDevice,  vertexShader, ShaderStageVertex);
    m_pRenderElements[m_RenderElementsCount].m_FragmentShader = ShadersVK::Load(m_pGraphicsDevice, fragmentShader, ShaderStageFragment);
    m_pRenderElements[m_RenderElementsCount].m_VertexShader = m_pVertexBuffers->CreateVertexBuffer(m_pGraphicsDevice, vertexInputState, size, pVertexData, numVertices);

    m_RenderElementsCount++;
    return m_RenderElementsCount - 1;
}

void Renderer::DrawRenderElement(uint32_t renderElement)
{
    Assert(m_RenderElementsIndexCount < scm_QueuedElementsBlock);
    m_pRenderElementIndices[m_RenderElementsIndexCount] = renderElement;
    m_RenderElementsIndexCount++;
}

