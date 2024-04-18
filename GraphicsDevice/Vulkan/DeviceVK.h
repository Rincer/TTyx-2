#ifndef _DEVICEVK_H_
#define _DEVICEVK_H_

#include "VertexBuffersVK.h"
#include "..\Memory\RingBuffer.h"

class DeviceState;
class ShadersVK;

class DeviceVK
{
    public:
        void Initialize();
        void Dispose();
        void CreateGraphicsPipeline(const DeviceState* pDeviceState, VkPipeline* pPipeline, const VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo);
        uint32_t BeginFrame();
        void EndFrame(uint32_t imageIndex);
        void BindPipeline(VkPipeline pipeline);
        void DrawVertexBuffer(VkBuffer buffer, VkDeviceSize offset, uint32_t numVerts);
        void WaitTillIdle();

        void SetVertexInput();
        void SetInputAssembly(VkPrimitiveTopology topology, uint32_t restartEnable);
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth);
        void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        void SetCullMode(VkCullModeFlagBits cullMode, VkFrontFace frontFace);
        void SetMultisampling();
        void SetDepthTesting();
        void SetStencilTesting();
        void SetAttachmentBlendState(uint32_t attachmentIndex);

        void GetSurfaceExtent(DeviceState& deviceState);
        VkShaderModule CreateShaderModule(const char* code, uint32_t codeSize);
        void DestroyShaderModule(VkShaderModule shaderModule);
        void CreateVertexBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, uint32_t size, const void* pData);
        void DestroyVertexBuffer(VertexBuffersVK::VertexBuffer* pBuffer);
        void SetDeviceState(const DeviceState* pDeviceState);
        
        static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
        static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    private:
        static const uint32_t scm_Frames = 2; // concurrent frames in flight
        static void* vkAllocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
        static void* vkReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
        static void vkFreeFunction(void* pUserData, void* pMemory);
        static void vkInternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
        static void vkInternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
        void InitializeExtensionFunctions();
        void InitializeAllocationCallbacks();
        void InitializeDebugUtilsMessengerCreateInfoEXT();
        void CreateInstance();
        void CreateSurfaceKHR();
        bool CheckEnabledLayers();
        bool CheckEnabledInstanceExtensions();
        static VkBool32 vkDebugUtilsMessengerCallbackEXT(
            VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
        void EnumeratePhysicalDevices();
        void SelectPhysicalDevice();
        void EnumerateQueueFamilies();
        void CreateLogicalDevice();
        void EnumerateDeviceExtensions(uint32_t deviceIndex);
        bool CheckDeviceExtensions(uint32_t deviceIndex);
        void QuerySurfaceInformation(VkPhysicalDevice device, VkSurfaceKHR surface);
        void CreateSwapChain(VkSurfaceKHR surface, VkFormat format, VkColorSpaceKHR colorSpace);
        void DisposeSwapChain();
        void CreateRenderPass();
        void CreatePipelineLayout();
        uint32_t GetMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void CreateStagingBuffer();
        void DisposeStagingBuffer();

        static const char* scm_EnabledLayers[];
        static const uint32_t scm_NumLayers;
        static const char* scm_InstanceExtensions[];
        static const uint32_t scm_NumInstanceExtensions;
        static const char* scm_DeviceExtensions[];
        static const uint32_t scm_NumDeviceExtensions;
        static const VkDynamicState scm_DynamicStates[];
        static const uint32_t scm_NumDynamicStates;

        VkInstance m_Instance;
        VkAllocationCallbacks m_AllocationCallbacks;
        VkDebugUtilsMessengerEXT m_DebugMessenger;      
        VkDebugUtilsMessengerCreateInfoEXT m_DebugUtilsMessengerCreateInfoEXT;
        uint32_t m_PhysicalDeviceCount;
        VkPhysicalDevice* m_pPhysicalDevices;
        uint32_t* m_pDeviceExtensionCounts;
        VkExtensionProperties** m_ppDeviceExtensions;
        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceMemoryProperties m_MemProperties;
        VkDevice m_LogicalDevice;        
        VkPhysicalDeviceProperties* m_pDeviceProperties;
        VkPhysicalDeviceFeatures* m_pDeviceFeatures;
        uint32_t m_QueueFamilyCount = 0;
        VkQueueFamilyProperties* m_pQueueFamilyProperties;
        uint32_t m_QueueFamilyIndex;  // all capabulities queue 
        VkQueue m_GraphicsQueue;
        VkSurfaceKHR m_Win32Surface;        
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        uint32_t m_SurfaceFormatCount;
        VkSurfaceFormatKHR* m_pSurfaceFormats;
        uint32_t m_PresentModeCount;
        VkPresentModeKHR* m_pSurfacePresentModes;

        VkSwapchainKHR m_SwapChain;
        uint32_t m_SwapChainImageCount;
        VkImage* m_pSwapChainImages;
        VkImageView* m_pSwapChainImageViews;     
        VkFramebuffer* m_pSwapChainFrameBuffers;

        ShadersVK* m_pShaders;
        
        static const uint32_t kRenderPassBlock = 32;
        VkRenderPass* m_pRenderPasses;
        uint32_t m_RenderPassCount;
        uint32_t m_CurrentRenderPass;
        
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer[scm_Frames];
        VkSemaphore m_ImageAvailableSemaphore[scm_Frames];
        VkSemaphore m_RenderFinishedSemaphore[scm_Frames];
        VkFence m_InFlightFence[scm_Frames];
        uint32_t m_DeviceFrameCount;
        uint32_t m_CurrFrame;

        static const uint32_t scm_StagingBufferSize = 128 * 1024;
        uint32_t m_StagingBufferOffsets[scm_Frames];
        RingBuffer m_StagingRingBuffer;
        VkBuffer m_StagingBuffer;
        VkDeviceMemory m_StagingBufferMemory;
        uint32_t m_CurrentStagingOffset;

        struct StagingBufferCopy
        {
            VkBufferCopy m_BufferCopy;
            VkBuffer m_DstBuffer;
        };

        static const uint32_t kStagingBufferCopiesBlock = 32;
        StagingBufferCopy* m_pStagingBufferCopies;        
        uint32_t m_StagingBufferCopyCount;
};

#endif