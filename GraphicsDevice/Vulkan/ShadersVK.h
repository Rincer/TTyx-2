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
            VkShaderModule m_ShaderModule;
        };

        void Initialize();
        void Dispose(DeviceVK* device);
        VkShaderModule GetShaderModule(uint32_t index, ShaderStageType stageType);
        static int Load(DeviceVK* device, const char* filePath, ShaderStageType stageType);

    private:
        
        static const uint32_t kShaderBlock = 32;
        static Shader*     m_Shaders[ShaderStageCount];
        static uint32_t    m_ShaderCounts[ShaderStageCount];
};

#endif


