#ifndef _SHADERSVK_H_
#define _SHADERSVK_H_

#include "..\DeviceState.h"
#include "..\..\ShaderUtils\SPIRV-reflect\spirv_reflect.h"
#include "..\ShaderConstants.h"

class DeviceVK;


class ShadersVK
{
    public:
        
        struct Shader
        {
            const char* m_Code;
            uint32_t m_CodeLength;
            uint64_t m_Hash;
            VkShaderModule m_ShaderModule;
            VkDescriptorSetLayout m_DescriptorSetLayout;
            VkDescriptorSet m_DescriptorSet;
            bool m_DescriptorSetLayoutValid;
        };

        void Initialize();
        void Dispose(DeviceVK* device);
        VkShaderModule GetShaderModule(uint32_t index, ShaderStageType stageType) const;
        VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t index, ShaderStageType stageType) const;
        VkDescriptorSet GetDescriptorSet(uint32_t index, ShaderStageType stageType) const;
        bool IsDescriptorSetLayoutValid(uint32_t index, ShaderStageType stageType) const;
        int Load(DeviceVK* device, const char* code, uint32_t codeLength, uint64_t hash, const ShaderConstants::BufferInfo* bufferInfos, uint32_t bufferCount, const SpvReflectShaderModule& module, ShaderStageType stageType);
        int GetIndex(uint64_t hash, ShaderStageType stageType) const;

    private:
        
        static const uint32_t kShaderBlock = 32;
        Shader*     m_Shaders[ShaderStageCount];
        uint32_t    m_ShaderCounts[ShaderStageCount];        
};

#endif


