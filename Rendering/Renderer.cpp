#include "stdafx.h"
#include "..\Memory\MemoryManager.h"
#include "..\GraphicsDevice\Vulkan\ShadersVK.h"
#include "..\GraphicsDevice\Vulkan\DeviceVK.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"
#include "..\Diagnostics\Debugging.h"
#include "..\Utilities\Hash.h"
#include "..\GraphicsDevice\ShaderConstants.h"

#include "Renderer.h"

void Renderer::Initialize()
{
    m_pGraphicsDevice = static_cast<DeviceVK*>(CMemoryManager::GetAllocator().Alloc(sizeof(DeviceVK)));
    m_pGraphicsDevice->Initialize();
    m_ResourceContext.Initialize();
    m_RenderElementsCount = 0;
    m_pRenderElements = static_cast<RenderElement*>(CMemoryManager::GetAllocator().Alloc(scm_RenderElementsBlock * sizeof(RenderElement)));
    m_pRenderElementIndices = static_cast<uint32_t*>(CMemoryManager::GetAllocator().Alloc(scm_QueuedElementsBlock * sizeof(uint32_t)));
    m_RenderElementsIndexCount = 0;    
    m_Rotation = 0.0f;
}

void Renderer::Dispose()
{
    m_pGraphicsDevice->WaitTillIdle();
    m_ResourceContext.Dispose(m_pGraphicsDevice);
    m_pGraphicsDevice->Dispose();
    CMemoryManager::GetAllocator().Free(m_pGraphicsDevice);
    CMemoryManager::GetAllocator().Free(m_pRenderElements);
    CMemoryManager::GetAllocator().Free(m_pRenderElementIndices);
    m_PipelineMap.clear();
}

void Renderer::DrawRenderElements()
{
    m_pGraphicsDevice->GetSurfaceExtent(m_DeviceState);
    m_DeviceState.SetColorBlend(0);
    m_DeviceState.SetCullMode(VK_CULL_MODE_BACK_BIT);
    m_DeviceState.SetFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    for (uint32_t i = 0; i < m_RenderElementsIndexCount; i++)
    {
        RenderElement& renderElement = m_pRenderElements[m_pRenderElementIndices[i]];
        m_DeviceState.SetShader(renderElement.m_VertexShader, ShaderStageVertex);
        m_DeviceState.SetShader(renderElement.m_FragmentShader, ShaderStageFragment);
        m_DeviceState.SetVertexBuffer(renderElement.m_VertexBuffer);
        // Index buffers do not affect pipeline state
        m_DeviceState.Update();
        std::unordered_map<uint64_t,PipelineAndLayout>::const_iterator it = m_PipelineMap.find(m_DeviceState.GetHash());
        PipelineAndLayout pipelineAndLayout{};
        if (it == m_PipelineMap.end())
        {
            m_pGraphicsDevice->CreateGraphicsPipeline(&m_DeviceState, &pipelineAndLayout.m_Pipeline, &pipelineAndLayout.m_Layout, &m_ResourceContext);
            m_PipelineMap.insert(std::make_pair(m_DeviceState.GetHash(), pipelineAndLayout));
        }
        else
        {
            pipelineAndLayout = it->second;
        }
        VkDescriptorSet descriptorSets[2];
        uint32_t descriptorCount = 0;
        for (uint32_t stage = ShaderStageVertex; stage <= ShaderStageFragment; stage++)
        {
            ShaderStageType stageType = static_cast<ShaderStageType>(stage);
            if (m_ResourceContext.m_Shaders.IsDescriptorSetLayoutValid(m_DeviceState.GetShader(stageType), stageType))
            {
                descriptorSets[descriptorCount] = m_ResourceContext.m_Shaders.GetDescriptorSet(m_DeviceState.GetShader(stageType), stageType);
                descriptorCount++;
            }
        }
        if (descriptorCount > 0)
        {
            m_pGraphicsDevice->BindDescriptorSets(pipelineAndLayout.m_Layout, descriptorSets, descriptorCount);
        }
        m_pGraphicsDevice->BindPipeline(pipelineAndLayout.m_Pipeline);
        m_pGraphicsDevice->DrawVertexBuffer(&m_ResourceContext, renderElement.m_VertexBuffer, renderElement.m_IndexBuffer);
    }
}

void Renderer::Execute(float frameTime)
{
    struct UniformBufferObject 
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } ubo;
    
    m_Rotation += glm::pi<float>() * frameTime;

    ubo.model = glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));    
    m_pGraphicsDevice->GetSurfaceExtent(m_DeviceState);
    ubo.proj = glm::perspective(glm::radians(45.0f), m_DeviceState.GetWidth() / (float)m_DeviceState.GetHeight(), 0.1f, 10.0f);

    m_ResourceContext.m_ShaderConstants.UpdateConstant(m_pGraphicsDevice, "ubo", "model", &ubo.model, sizeof(ubo.model));
    m_ResourceContext.m_ShaderConstants.UpdateConstant(m_pGraphicsDevice, "ubo", "view", &ubo.view, sizeof(ubo.view));
    m_ResourceContext.m_ShaderConstants.UpdateConstant(m_pGraphicsDevice, "ubo", "proj", &ubo.proj, sizeof(ubo.proj));

    uint32_t imageIndex = m_pGraphicsDevice->BeginFrame();
    if (imageIndex != -1)
    {
        DrawRenderElements();
        m_pGraphicsDevice->EndFrame(imageIndex);        
    }    
    m_RenderElementsIndexCount = 0;
}

uint32_t Renderer::LoadShader(const char* fileName, ShaderStageType shaderStage)
{
    uint64_t hash = Hash::Hash64(fileName);
    uint32_t index = m_ResourceContext.m_Shaders.GetIndex(hash, ShaderStageVertex);
    if (index == -1)
    {
        FILE* f = nullptr;
        fopen_s(&f, fileName, "rb");
        if (f != nullptr)
        {
            uint32_t codeLength = _filelength(_fileno(f));
            char* code = static_cast<char*>(CMemoryManager::GetAllocator().Alloc(codeLength));
            fread_s(code, codeLength, 1, codeLength, f);
            fclose(f);
            SpvReflectShaderModule module = {};
            SpvReflectResult result = spvReflectCreateShaderModule(codeLength, code, &module);
            Assert(result == SPV_REFLECT_RESULT_SUCCESS);

            m_ResourceContext.m_ShaderConstants.AddConstantBuffers(m_pGraphicsDevice, module);
            const ShaderConstants::BufferInfo* bufferInfos;
            uint32_t numBuffers;
            m_ResourceContext.m_ShaderConstants.GetBufferInfos(bufferInfos, numBuffers);
            index = m_ResourceContext.m_Shaders.Load(m_pGraphicsDevice, code, codeLength, hash, bufferInfos, numBuffers, module, shaderStage);
            spvReflectDestroyShaderModule(&module);
        }
    }
    return index;
}

uint32_t Renderer::CreateRenderElement(const char* vertexShader, const char* fragmentShader, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t size, const void* pData, uint32_t numVertices)
{
    Assert(m_RenderElementsCount < scm_RenderElementsBlock);
    m_pRenderElements[m_RenderElementsCount].m_VertexShader = LoadShader(vertexShader, ShaderStageVertex);
    m_pRenderElements[m_RenderElementsCount].m_FragmentShader = LoadShader(fragmentShader, ShaderStageFragment);
    m_pRenderElements[m_RenderElementsCount].m_VertexBuffer = m_ResourceContext.m_VertexBuffers.CreateVertexBuffer(m_pGraphicsDevice, vertexInputState, size, pData, numVertices);
    m_pRenderElements[m_RenderElementsCount].m_IndexBuffer = -1;
    m_RenderElementsCount++;
    return m_RenderElementsCount - 1;
}

uint32_t Renderer::CreateIndexedRenderElement(const char* vertexShader, const char* fragmentShader, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t vertexDataSize, const void* pVertexData, uint32_t numVertices,
    uint32_t indexDataSize, const void* pIndexData, uint32_t numIndices)
{
    Assert(m_RenderElementsCount < scm_RenderElementsBlock);
    m_pRenderElements[m_RenderElementsCount].m_VertexShader = LoadShader(vertexShader, ShaderStageVertex);
    m_pRenderElements[m_RenderElementsCount].m_FragmentShader = LoadShader(fragmentShader, ShaderStageFragment);
    m_pRenderElements[m_RenderElementsCount].m_VertexBuffer = m_ResourceContext.m_VertexBuffers.CreateVertexBuffer(m_pGraphicsDevice, vertexInputState, vertexDataSize, pVertexData, numVertices);
    m_pRenderElements[m_RenderElementsCount].m_IndexBuffer = m_ResourceContext.m_IndexBuffers.CreateIndexBuffer(m_pGraphicsDevice, indexDataSize, pIndexData, numIndices);;
    m_RenderElementsCount++;
    return m_RenderElementsCount - 1;
}

void Renderer::DrawRenderElement(uint32_t renderElement)
{
    Assert(m_RenderElementsIndexCount < scm_QueuedElementsBlock);
    m_pRenderElementIndices[m_RenderElementsIndexCount] = renderElement;
    m_RenderElementsIndexCount++;
}


