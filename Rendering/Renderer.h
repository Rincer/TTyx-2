#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "..\GraphicsDevice\DeviceState.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"
#include "..\GraphicsDevice\Vulkan\IndexBuffersVK.h"
#include "..\Memory\MemoryManager.h"
#include "ResourceContext.h"
#include <unordered_map>

class DeviceVK;

class Renderer
{
    public:
        struct RenderElement
        {
            uint32_t m_VertexShader;
            uint32_t m_FragmentShader;
            uint32_t m_VertexBuffer;
            uint32_t m_IndexBuffer;
        };
        void Initialize();
        void Dispose();
        void Execute();                
        uint32_t CreateRenderElement(const char* vertexShader, const char* fragmentShader, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t size, const void* pData, uint32_t numVertices);
        uint32_t CreateIndexedRenderElement(const char* vertexShader, const char* fragmentShader, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t vertexDataSize, const void* pVertexData, uint32_t numVertices,
            uint32_t indexDataSize, const void* pIndexData, uint32_t numIndices);
        void DrawRenderElement(uint32_t renderElement);

    private:        

        void DrawRenderElements();
        DeviceState m_DeviceState;
        DeviceVK* m_pGraphicsDevice;
        ResourceContext m_ResourceContext;
        std::unordered_map<uint64_t, VkPipeline, std::hash<uint64_t>, std::equal_to<uint64_t>, StdAllocator<std::pair<uint64_t, VkPipeline>>> m_PipelineMap;

        static const uint32_t scm_RenderElementsBlock = 32;
        RenderElement* m_pRenderElements;
        uint32_t m_RenderElementsCount;

        static const uint32_t scm_QueuedElementsBlock = 32;
        uint32_t* m_pRenderElementIndices;
        uint32_t m_RenderElementsIndexCount;
};

#endif
