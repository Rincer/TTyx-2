#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "..\GraphicsDevice\DeviceState.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"
#include "..\Memory\MemoryManager.h"
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
        };
        void Initialize();
        void Dispose();
        void Execute();                
        uint32_t CreateRenderElement(const char* vertexShader, const char* fragmentShader, const VertexBuffersVK::VertexInputState& vertexInputState, uint32_t size, const void* pVertexData, uint32_t numVertices);
        void DrawRenderElement(uint32_t renderElement);

    private:        

        void DrawRenderElements();
        DeviceState m_DeviceState;
        DeviceVK* m_pGraphicsDevice;
        VertexBuffersVK* m_pVertexBuffers;        
        std::unordered_map<uint64_t, VkPipeline, std::hash<uint64_t>, std::equal_to<uint64_t>, StdAllocator<std::pair<uint64_t, VkPipeline>>> m_PipelineMap;

        static const uint32_t scm_RenderElementsBlock = 32;
        RenderElement* m_pRenderElements;
        uint32_t m_RenderElementsCount;

        static const uint32_t scm_QueuedElementsBlock = 32;
        uint32_t* m_pRenderElementIndices;
        uint32_t m_RenderElementsIndexCount;
};

#endif
