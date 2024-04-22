#ifndef INDEXBUFFERS_VK_H
#define INDEXBUFFERS_VK_H

class DeviceVK;

class IndexBuffersVK
{
    public:

        void Initialize();
        void Dispose(DeviceVK* pGraphicsDevice);
        uint32_t CreateIndexBuffer(DeviceVK* pGraphicsDevice, uint32_t size, const void* pData, uint32_t numIndices);
        void GetIndexBuffer(VkBuffer& buffer, uint32_t& numIndices, uint32_t indexBufferIndex);

        struct IndexBuffer
        {
            VkBuffer        m_IndexBuffer;
            VkDeviceMemory  m_BufferMemory;
            uint32_t        m_NumIndices;
        };

    private:

        static const uint32_t scm_IndexBuffersBlock = 32;
        IndexBuffer* m_pIndexBuffers;
        uint32_t m_IndexBuffersCount;
};

#endif
