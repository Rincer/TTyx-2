#ifndef VERTEXBUFFERS_VK_H
#define VERTEXBUFFERS_VK_H

class DeviceVK;

class VertexBuffersVK
{
    public:
        class VertexInputState
        {
            public:
                VertexInputState();

                void AddBinding(uint32_t slot, uint32_t stride);
                void AddAttribute(uint32_t slot, uint32_t location, VkFormat format, uint32_t offset);
                const VkPipelineVertexInputStateCreateInfo& GetVertexInput();

            private:
                static const int scm_MaxBindings = 4;
                static const int scm_MaxDescriptors = 6;                
                VkVertexInputBindingDescription         m_Bindings[scm_MaxBindings];
                uint32_t                                m_BindingsCount;
                VkVertexInputAttributeDescription       m_Attributes[scm_MaxDescriptors];
                uint32_t                                m_AttributesCount;
                VkPipelineVertexInputStateCreateInfo    m_VertexInput;
                uint32_t                                m_NumVerts;

                friend class VertexBuffersVK;
        };

        void Initialize();
        void Dispose(DeviceVK* pGraphicsDevice);
        uint32_t CreateVertexBuffer(DeviceVK* pGraphicsDevice, const VertexInputState& vertexInputState, uint32_t size, const void* pData, uint32_t numVertices);
        const VkPipelineVertexInputStateCreateInfo& GetVertexInputCreateInfo(uint32_t vertexBufferIndex) const;
        void GetVertexBuffer(VkBuffer& buffer, uint32_t& numVertices, uint32_t vertexBufferIndex);

        struct VertexBuffer
        {
            VkBuffer            m_VertexBuffer;
            VkDeviceMemory      m_BufferMemory;
            VertexInputState    m_InputState;            
        };

    private:        

        static const uint32_t scm_VertexBuffersBlock = 32;
        VertexBuffer* m_pVertexBuffers;
        uint32_t m_VertexBuffersCount;
};

#endif
