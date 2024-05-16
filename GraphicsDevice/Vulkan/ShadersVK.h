#ifndef _SHADERSVK_H_
#define _SHADERSVK_H_

#include "..\DeviceState.h"

class DeviceVK;

class ShadersVK
{
    public:
        
        struct Shader
        {
            char* m_FilePath;
            char* m_Code;
            uint32_t m_CodeLength;
            uint64_t m_Hash;
            VkShaderModule m_ShaderModule;
        };

        void Initialize();
        void Dispose(DeviceVK* device);
        VkShaderModule GetShaderModule(uint32_t index, ShaderStageType stageType) const;
        int Load(DeviceVK* device, const char* filePath, ShaderStageType stageType);

    private:
        
        static const uint32_t kShaderBlock = 32;
        Shader*     m_Shaders[ShaderStageCount];
        uint32_t    m_ShaderCounts[ShaderStageCount];
};

#endif


