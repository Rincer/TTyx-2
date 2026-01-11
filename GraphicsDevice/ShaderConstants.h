#ifndef _SHADERCONSTANTS_H_
#define _SHADERCONSTANTS_H_

#include "../Memory/MemoryManager.h"
#include "../ShaderUtils/SPIRV-reflect/spirv_reflect.h"

class DeviceVK;

class ShaderConstants
{
    public:
        static const uint32_t scm_MaxNameLength = 256;
        struct BufferInfo
        {
            char            m_Name[scm_MaxNameLength];
            uint32_t        m_Size;
            VkBuffer        m_buffer;
            VkDeviceMemory  m_BufferMemory;
        };
        void Initialize();
        void Dispose(DeviceVK* pDevice);
        void AddConstantBuffers(DeviceVK* pDevice, const SpvReflectShaderModule& module);
        void UpdateConstant(DeviceVK* pDevice, const char* pBufferName, const char* pConstantName, void* pData, uint32_t size);
        void GetBufferInfos(const BufferInfo*& bufferInfos, uint32_t& numBuffers) const;

    private:

        struct Constant
        {
            char     m_Name[scm_MaxNameLength];
            uint32_t m_Offset;
            uint32_t m_Size;
        };

        static const uint32_t scm_MaxConstants = 32;

        struct ConstantBuffer
        {
            uint32_t        m_NumConstants;
            Constant        m_Constants[scm_MaxConstants];
        };

        static const uint32_t scm_ConstantBufferBlock = 32;
        ConstantBuffer m_ConstantBuffers[scm_ConstantBufferBlock];
        BufferInfo m_BufferInfos[scm_ConstantBufferBlock];
        uint32_t m_NumBuffers;
        
};

#endif
