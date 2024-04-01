#ifndef _GRAPHICSPIPELINES_VK_H
#define _GRAPHICSPIPELINES_VK_H

#include "../DeviceState.h"
#include "../../Memory/MemoryManager.h"
#include <unordered_map>

class GraphicsPipelinesVK
{
    public:
        void Initialize();
        void Dispose();

    private:
        std::unordered_map<DeviceState, VkPipeline, std::hash<uint32_t>, std::equal_to<uint32_t>, StdAllocator<std::pair<DeviceState, VkPipeline>>> m_PipelineMap;
};

#endif
