#include "stdafx.h"
#include "../../Memory/MemoryManager.h"
#include "../../Diagnostics/Debugging.h"
#include "../../Windows/Window.h"
#include "../../Utilities/Macros.h"
#include "../../Rendering/ResourceContext.h"
#include "ShadersVK.h"

#include "DeviceVK.h"

VkBool32 DeviceVK::vkDebugUtilsMessengerCallbackEXT(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity)
    {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        {
            DebugPrintf("vkDebugError: %s\n", pCallbackData->pMessage);
            break;
        }
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        {
            DebugPrintf("vkDebugWarning: %s\n", pCallbackData->pMessage);
            break;
        }
        default:
            break;
    }
    return VK_FALSE;
}

PFN_vkCreateDebugUtilsMessengerEXT DeviceVK::vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT DeviceVK::vkDestroyDebugUtilsMessengerEXT = nullptr;

void DeviceVK::InitializeExtensionFunctions()
{
    vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
}

void DeviceVK::InitializeAllocationCallbacks()
{
    m_AllocationCallbacks.pfnAllocation = vkAllocationFunction;
    m_AllocationCallbacks.pfnFree = vkFreeFunction;
    m_AllocationCallbacks.pfnReallocation = vkReallocationFunction;
    m_AllocationCallbacks.pfnInternalAllocation = vkInternalAllocationNotification;
    m_AllocationCallbacks.pfnInternalFree = vkInternalFreeNotification;
}

void DeviceVK::InitializeDebugUtilsMessengerCreateInfoEXT()
{
    m_DebugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    m_DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    m_DebugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    m_DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = vkDebugUtilsMessengerCallbackEXT;
    m_DebugUtilsMessengerCreateInfoEXT.pUserData = nullptr;
}

const char* DeviceVK::scm_InstanceExtensions[] =
{
    "VK_KHR_device_group_creation",
    "VK_KHR_display", 
    "VK_KHR_external_fence_capabilities",
    "VK_KHR_external_memory_capabilities",
    "VK_KHR_external_semaphore_capabilities",
    "VK_KHR_get_display_properties2",
    "VK_KHR_get_physical_device_properties2", 
    "VK_KHR_get_surface_capabilities2", 
    "VK_KHR_surface", 
//    "VK_KHR_surface_protected_capabilities", 
    "VK_KHR_win32_surface", 
    "VK_EXT_debug_report", 
    "VK_EXT_debug_utils", 
    "VK_EXT_direct_mode_display", 
    "VK_EXT_swapchain_colorspace", 
//    "VK_NV_external_memory_capabilities"
};
const uint32_t DeviceVK::scm_NumInstanceExtensions = sizeof(scm_InstanceExtensions) / sizeof(char*);

const char* DeviceVK::scm_DeviceExtensions[] =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const uint32_t DeviceVK::scm_NumDeviceExtensions = sizeof(scm_DeviceExtensions) / sizeof(char*);


const char* DeviceVK::scm_EnabledLayers[] =
{
    "VK_LAYER_KHRONOS_validation"
};
const uint32_t DeviceVK::scm_NumLayers = sizeof(scm_EnabledLayers) / sizeof(char*);

const VkDynamicState DeviceVK::scm_DynamicStates[] =
{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

const uint32_t DeviceVK::scm_NumDynamicStates = sizeof(scm_DynamicStates) / sizeof(VkDynamicState);

void DeviceVK::CreateInstance()
{
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = scm_NumInstanceExtensions;
    createInfo.ppEnabledExtensionNames = scm_InstanceExtensions;
    createInfo.enabledLayerCount = scm_NumLayers;
    createInfo.ppEnabledLayerNames = scm_EnabledLayers;
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&m_DebugUtilsMessengerCreateInfoEXT;  // so that validation messages can be intercepted by the callback
    VkResult result = vkCreateInstance(&createInfo, &m_AllocationCallbacks, &m_Instance);
    Assert(result == VK_SUCCESS);
}

void DeviceVK::CreateSurfaceKHR()
{
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = CWindow::GetCurrentViewWnd();
    createInfo.hinstance = GetModuleHandle(nullptr);
    VkResult result = vkCreateWin32SurfaceKHR(m_Instance, &createInfo, &m_AllocationCallbacks, &m_Win32Surface);
    Assert(result == VK_SUCCESS);
}

bool DeviceVK::CheckEnabledLayers()
{
    uint32_t layerPropertyCount = 0;
    vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);
    VkLayerProperties* pLayerProperties;
    pLayerProperties = (VkLayerProperties*)CMemoryManager::GetAllocator().Alloc(layerPropertyCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerPropertyCount, pLayerProperties);
    bool layerFound;
    for (uint32_t i = 0; i < scm_NumLayers; i++)
    {
        layerFound = false;
        for (uint32_t j = 0; j < layerPropertyCount; j++)
        {
            if (!_stricmp(scm_EnabledLayers[i], pLayerProperties[j].layerName))
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            break;
    }
    CMemoryManager::GetAllocator().Free(pLayerProperties);
    return layerFound;
}

bool DeviceVK::CheckEnabledInstanceExtensions()
{    
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    VkExtensionProperties* pExtensionProperties;
    pExtensionProperties = (VkExtensionProperties*)CMemoryManager::GetAllocator().Alloc(extensionCount * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, pExtensionProperties);
    int numExtensionsFound = 0;
    for (uint32_t i = 0; i < scm_NumInstanceExtensions; i++)
    {
        for (uint32_t j = 0; j < extensionCount; j++)
        {
            if (!_stricmp(scm_InstanceExtensions[i], pExtensionProperties[j].extensionName))
            {
                numExtensionsFound++;
                break;
            }
        }
    }
    CMemoryManager::GetAllocator().Free(pExtensionProperties);
    return numExtensionsFound == scm_NumInstanceExtensions;
}

void DeviceVK::EnumerateDeviceExtensions(uint32_t deviceIndex)
{
    vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[deviceIndex], nullptr, &m_pDeviceExtensionCounts[deviceIndex], nullptr);
    m_ppDeviceExtensions[deviceIndex] = static_cast<VkExtensionProperties*>(CMemoryManager::GetAllocator().Alloc(m_pDeviceExtensionCounts[deviceIndex] * sizeof(VkExtensionProperties)));
    vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[deviceIndex], nullptr, &m_pDeviceExtensionCounts[deviceIndex], m_ppDeviceExtensions[deviceIndex]);
}

void DeviceVK::EnumeratePhysicalDevices()
{
    vkEnumeratePhysicalDevices(m_Instance, &m_PhysicalDeviceCount, nullptr);
    m_pPhysicalDevices = static_cast<VkPhysicalDevice*>(CMemoryManager::GetAllocator().Alloc(m_PhysicalDeviceCount * sizeof(VkPhysicalDevice)));
    m_pDeviceProperties = static_cast<VkPhysicalDeviceProperties*>(CMemoryManager::GetAllocator().Alloc(m_PhysicalDeviceCount * sizeof(VkPhysicalDeviceProperties)));
    m_pDeviceFeatures = static_cast<VkPhysicalDeviceFeatures*>(CMemoryManager::GetAllocator().Alloc(m_PhysicalDeviceCount * sizeof(VkPhysicalDeviceFeatures)));
    m_pDeviceExtensionCounts = static_cast<uint32_t*>(CMemoryManager::GetAllocator().Alloc(m_PhysicalDeviceCount * sizeof(uint32_t)));
    m_ppDeviceExtensions = static_cast<VkExtensionProperties**>(CMemoryManager::GetAllocator().Alloc(m_PhysicalDeviceCount * sizeof(VkExtensionProperties*)));
    vkEnumeratePhysicalDevices(m_Instance, &m_PhysicalDeviceCount, m_pPhysicalDevices);
    for (uint32_t i = 0; i < m_PhysicalDeviceCount; i++)
    {
        vkGetPhysicalDeviceProperties(m_pPhysicalDevices[i], &m_pDeviceProperties[i]);
        vkGetPhysicalDeviceFeatures(m_pPhysicalDevices[i], &m_pDeviceFeatures[i]);
        EnumerateDeviceExtensions(i);
    }    
}

bool DeviceVK::CheckDeviceExtensions(uint32_t deviceIndex)
{
    VkExtensionProperties* pDeviceExtensions = m_ppDeviceExtensions[deviceIndex];
    uint32_t numSupportedExtensions = 0;
    for (uint32_t i = 0; i < scm_NumDeviceExtensions; i++)
    {
        for (uint32_t j = 0; j < m_pDeviceExtensionCounts[deviceIndex]; j++)
        {
            if (!_stricmp(scm_DeviceExtensions[i], pDeviceExtensions[j].extensionName))
            {
                numSupportedExtensions++;
                break;
            }
        }
    }
    // All required device extensions must be supported
    return numSupportedExtensions == scm_NumDeviceExtensions;
}

void DeviceVK::QuerySurfaceInformation(VkPhysicalDevice device, VkSurfaceKHR surface)
{    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Win32Surface, &m_SurfaceCapabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &m_SurfaceFormatCount, nullptr);
    m_pSurfaceFormats = static_cast<VkSurfaceFormatKHR*>(CMemoryManager::GetAllocator().Alloc(sizeof(VkSurfaceFormatKHR) * m_SurfaceFormatCount));
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &m_SurfaceFormatCount, m_pSurfaceFormats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &m_PresentModeCount, nullptr);
    m_pSurfacePresentModes = static_cast<VkPresentModeKHR*>(CMemoryManager::GetAllocator().Alloc(sizeof(VkPresentModeKHR) * m_PresentModeCount));
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &m_PresentModeCount, m_pSurfacePresentModes);
}

void DeviceVK::CreateSwapChain(VkSurfaceKHR surface, VkFormat format, VkColorSpaceKHR colorSpace)
{    
    uint32_t i;
    // Check that format is supported
    for (i = 0; i < m_SurfaceFormatCount; i++)
    {
        if (m_pSurfaceFormats[i].format == format)
            break;
    }
    Assert(i < m_SurfaceFormatCount);
    //Check that colorspace is supported
    for (i = 0; i < m_SurfaceFormatCount; i++)
    {
        if (m_pSurfaceFormats[i].colorSpace == colorSpace)
            break;
    }
    Assert(i < m_SurfaceFormatCount);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = m_SurfaceCapabilities.minImageCount + 1; // +1 to avoid stalls
    createInfo.imageFormat = format;
    createInfo.imageColorSpace = colorSpace;
    createInfo.imageExtent = m_SurfaceCapabilities.currentExtent;
    createInfo.imageArrayLayers = 1;    // 1 unless stereoscopic
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;    // rendering directly to onscreen image, change to VK_IMAGE_USAGE_TRANSFER_DST_BIT if rendering to a an offscreen surface
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;        // presenting exclusively on the graphics queue `DeviceVK::EnumerateQueueFamilies()`
    createInfo.queueFamilyIndexCount = 0; 
    createInfo.pQueueFamilyIndices = nullptr; 
    createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // no blending with other windows
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;              // vertical sync mode, guaranteed to be available
    createInfo.clipped = VK_TRUE;                                   // other windows can clip this window
    createInfo.oldSwapchain = VK_NULL_HANDLE;                       // Creating the swapchain for the 1st time
    VkResult result = vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, &m_AllocationCallbacks, &m_SwapChain);
    Assert(result == VK_SUCCESS);
    vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &m_SwapChainImageCount, nullptr);

    m_pSwapChainImages = static_cast<VkImage*>(CMemoryManager::GetAllocator().Alloc(m_SwapChainImageCount * sizeof(VkImage)));
    vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &m_SwapChainImageCount, m_pSwapChainImages);
    m_pSwapChainImageViews = static_cast<VkImageView*>(CMemoryManager::GetAllocator().Alloc(m_SwapChainImageCount * sizeof(VkImageView)));
    m_pSwapChainFrameBuffers = static_cast<VkFramebuffer*>(CMemoryManager::GetAllocator().Alloc(m_SwapChainImageCount * sizeof(VkFramebuffer)));
    
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;    
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_pRenderPasses[m_CurrentRenderPass];
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.width = m_SurfaceCapabilities.currentExtent.width;
    framebufferInfo.height = m_SurfaceCapabilities.currentExtent.height;
    framebufferInfo.layers = 1;

    for (uint32_t i = 0; i < m_SwapChainImageCount; i++)
    {
        imageViewCreateInfo.image = m_pSwapChainImages[i];
        result = vkCreateImageView(m_LogicalDevice, &imageViewCreateInfo, &m_AllocationCallbacks, &m_pSwapChainImageViews[i]);
        Assert(result == VK_SUCCESS);
        framebufferInfo.pAttachments = &m_pSwapChainImageViews[i];
        result = vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, &m_AllocationCallbacks, &m_pSwapChainFrameBuffers[i]);
        Assert(result == VK_SUCCESS);
    }    
}

void DeviceVK::CreateRenderPass()
{
    // Create the render pass
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // This is the render pass that waits on swapchain image to become available
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;    // implicit subpass that happens before this renderpass starts
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;    // The last stage
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;            // All operations that involve writing should wait

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    Assert(m_RenderPassCount < kRenderPassBlock);    
    m_pRenderPasses = static_cast<VkRenderPass*>(CMemoryManager::GetAllocator().Alloc(kRenderPassBlock * sizeof(VkRenderPass)));
    m_CurrentRenderPass = m_RenderPassCount;
    VkResult res = vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, &m_AllocationCallbacks, &m_pRenderPasses[m_RenderPassCount]);
    Assert(res == VK_SUCCESS);
    m_RenderPassCount++;
}

void DeviceVK::CreatePipelineLayout()
{

}

uint32_t DeviceVK::GetMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    const uint32_t memoryCount = m_MemProperties.memoryTypeCount;
    for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) 
    {
        const uint32_t memoryTypeBits = (1 << memoryIndex);
        const bool isRequiredMemoryType = typeFilter & memoryTypeBits;

        const VkMemoryPropertyFlags propertyFlags = m_MemProperties.memoryTypes[memoryIndex].propertyFlags;
        const bool hasRequiredProperties = (propertyFlags & properties) == properties;

        if (isRequiredMemoryType && hasRequiredProperties)
            return memoryIndex;
    }
    // failed to find memory type
    return -1;
}

void DeviceVK::CreateStagingBuffer()
{
    VkDeviceSize bufferSize = scm_StagingBufferSize;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult res = vkCreateBuffer(m_LogicalDevice, &bufferInfo, &m_AllocationCallbacks, &m_StagingBuffer);
    Assert(res == VK_SUCCESS);
    m_StagingRingBuffer.Initialize(scm_StagingBufferSize);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_LogicalDevice, m_StagingBuffer, &memRequirements);
    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memRequirements.size;
    memoryAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    res = vkAllocateMemory(m_LogicalDevice, &memoryAllocInfo, &m_AllocationCallbacks, &m_StagingBufferMemory);
    Assert(res == VK_SUCCESS);
    vkBindBufferMemory(m_LogicalDevice, m_StagingBuffer, m_StagingBufferMemory, 0);
    for (uint32_t i = 0; i < scm_Frames; i++)
    {
        m_StagingBufferOffsets[i] = -1; // when -1 we skip staging ring buffer update in 'BeginFrame'
    }

    static const uint32_t kStagingBufferCopiesBlock = 32;
    m_pStagingBufferCopies = static_cast<StagingBufferCopy*>(CMemoryManager::GetAllocator().Alloc(kStagingBufferCopiesBlock * sizeof(StagingBufferCopy)));
    m_StagingBufferCopyCount = 0;
}

void DeviceVK::DisposeStagingBuffer()
{
    vkDestroyBuffer(m_LogicalDevice, m_StagingBuffer, &m_AllocationCallbacks);
    vkFreeMemory(m_LogicalDevice, m_StagingBufferMemory, &m_AllocationCallbacks);
    CMemoryManager::GetAllocator().Free(m_pStagingBufferCopies);
}

void DeviceVK::CreateLocalBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VkBufferCreateInfo& bufferInfo, uint32_t size, const void* pData)
{
    VkResult res = vkCreateBuffer(m_LogicalDevice, &bufferInfo, &m_AllocationCallbacks, &buffer);
    Assert(res == VK_SUCCESS);
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memRequirements);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetMemoryTypeIndex(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // GPU memory
    res = vkAllocateMemory(m_LogicalDevice, &allocInfo, &m_AllocationCallbacks, &bufferMemory);
    Assert(res == VK_SUCCESS);
    vkBindBufferMemory(m_LogicalDevice, buffer, bufferMemory, 0);

    uint32_t mapOffset = m_StagingRingBuffer.Allocate(size, m_CurrentStagingOffset);

    void* data;
    vkMapMemory(m_LogicalDevice, m_StagingBufferMemory, mapOffset, bufferInfo.size, 0, &data);
    memcpy(data, pData, (size_t)bufferInfo.size);
    vkUnmapMemory(m_LogicalDevice, m_StagingBufferMemory);

    Assert(m_StagingBufferCopyCount < kStagingBufferCopiesBlock);
    m_pStagingBufferCopies[m_StagingBufferCopyCount].m_DstBuffer = buffer;
    m_pStagingBufferCopies[m_StagingBufferCopyCount].m_BufferCopy.dstOffset = 0;
    m_pStagingBufferCopies[m_StagingBufferCopyCount].m_BufferCopy.size = size;
    m_pStagingBufferCopies[m_StagingBufferCopyCount].m_BufferCopy.srcOffset = mapOffset;
    m_StagingBufferCopyCount++;
}

void DeviceVK::SelectPhysicalDevice()
{
    for (uint32_t i = 0; i < m_PhysicalDeviceCount; i++)
    {
        if (CheckDeviceExtensions(i))
        {
            m_PhysicalDevice = m_pPhysicalDevices[i];
        }
    }
}

void DeviceVK::EnumerateQueueFamilies()
{
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &m_QueueFamilyCount, nullptr);
    m_pQueueFamilyProperties = static_cast<VkQueueFamilyProperties*>(CMemoryManager::GetAllocator().Alloc(m_QueueFamilyCount * sizeof(VkQueueFamilyProperties)));
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &m_QueueFamilyCount, m_pQueueFamilyProperties);
    // only graphics queue for now.
    for (uint32_t i = 0; i < m_QueueFamilyCount; i++)
    {
        if (m_pQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Win32Surface, &presentSupport);
            // We need to be able to present on the graphics queue
            Assert(presentSupport == 1);
            m_QueueFamilyIndex = i;
            break;
        }
    }
}

void DeviceVK::CreateLogicalDevice()
{
    VkPhysicalDeviceFeatures deviceFeatures {};    
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;
    queueCreateInfo.queueCount = 1;   
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledLayerCount = scm_NumLayers;
    createInfo.ppEnabledLayerNames = scm_EnabledLayers;
    createInfo.enabledExtensionCount = scm_NumDeviceExtensions;
    createInfo.ppEnabledExtensionNames = scm_DeviceExtensions;
    VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, &m_AllocationCallbacks, &m_LogicalDevice);
    Assert(result == VK_SUCCESS);
    vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndex, 0, &m_GraphicsQueue);
}

void DeviceVK::Initialize()
{   
    InitializeAllocationCallbacks();
    Assert(CheckEnabledInstanceExtensions());
    Assert(CheckEnabledLayers());
    InitializeDebugUtilsMessengerCreateInfoEXT();
    CreateInstance();
    CreateSurfaceKHR();
    InitializeExtensionFunctions();
    vkCreateDebugUtilsMessengerEXT(m_Instance, &m_DebugUtilsMessengerCreateInfoEXT, &m_AllocationCallbacks, &m_DebugMessenger);
    EnumeratePhysicalDevices();
    SelectPhysicalDevice();
    EnumerateQueueFamilies();    
    CreateLogicalDevice();
    QuerySurfaceInformation(m_PhysicalDevice, m_Win32Surface);
    // Need to be created before the swapchain, because it needs a renderpass in frame buffer creation    
    m_RenderPassCount = 0;
    CreateRenderPass();
    CreateSwapChain(m_Win32Surface, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemProperties);
          
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_QueueFamilyIndex;
    VkResult res = vkCreateCommandPool(m_LogicalDevice, &poolInfo, &m_AllocationCallbacks, &m_CommandPool);
    Assert(res == VK_SUCCESS);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // First time it is signalled.

    for (uint32_t i = 0; i < scm_Frames; i++)
    {
        res = vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &m_CommandBuffer[i]);
        Assert(res == VK_SUCCESS);
        res = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, &m_AllocationCallbacks, &m_ImageAvailableSemaphore[i]);
        Assert(res == VK_SUCCESS);
        res = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, &m_AllocationCallbacks, &m_RenderFinishedSemaphore[i]);
        Assert(res == VK_SUCCESS);
        res = vkCreateFence(m_LogicalDevice, &fenceInfo, &m_AllocationCallbacks, &m_InFlightFence[i]);
        Assert(res == VK_SUCCESS);
    }
    m_DeviceFrameCount = 0;
    CreateStagingBuffer();
}

void DeviceVK::DisposeSwapChain()
{
    vkDeviceWaitIdle(m_LogicalDevice);
    for (uint32_t i = 0; i < m_SwapChainImageCount; i++)
    {
        vkDestroyImageView(m_LogicalDevice, m_pSwapChainImageViews[i], &m_AllocationCallbacks);
        vkDestroyFramebuffer(m_LogicalDevice, m_pSwapChainFrameBuffers[i], &m_AllocationCallbacks);
    }
    vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, &m_AllocationCallbacks);
    CMemoryManager::GetAllocator().Free(m_pSwapChainImages);
    CMemoryManager::GetAllocator().Free(m_pSwapChainImageViews);
    CMemoryManager::GetAllocator().Free(m_pSwapChainFrameBuffers);
}

void DeviceVK::Dispose()
{    
    for (uint32_t i = 0; i < scm_Frames; i++)
    {
        vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphore[i], &m_AllocationCallbacks);
        vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphore[i], &m_AllocationCallbacks);
        vkDestroyFence(m_LogicalDevice, m_InFlightFence[i], &m_AllocationCallbacks);
    }
    vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, &m_AllocationCallbacks);
    DisposeSwapChain();   
    DisposeStagingBuffer();
        
    for (uint32_t i = 0; i < m_RenderPassCount; i++)
    {
        vkDestroyRenderPass(m_LogicalDevice, m_pRenderPasses[i], &m_AllocationCallbacks);
    }
           
    vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, &m_AllocationCallbacks);
    vkDestroyDevice(m_LogicalDevice, &m_AllocationCallbacks);
    vkDestroySurfaceKHR(m_Instance, m_Win32Surface, &m_AllocationCallbacks);
    vkDestroyInstance(m_Instance, &m_AllocationCallbacks);
    CMemoryManager::GetAllocator().Free(m_pDeviceExtensionCounts);
    for (uint32_t deviceIndex = 0; deviceIndex < m_PhysicalDeviceCount; deviceIndex++)
    {
        CMemoryManager::GetAllocator().Free(m_ppDeviceExtensions[deviceIndex]);
    }
    CMemoryManager::GetAllocator().Free(m_ppDeviceExtensions);
    CMemoryManager::GetAllocator().Free(m_pPhysicalDevices);
    CMemoryManager::GetAllocator().Free(m_pDeviceProperties);
    CMemoryManager::GetAllocator().Free(m_pDeviceFeatures);
    CMemoryManager::GetAllocator().Free(m_pQueueFamilyProperties);
    CMemoryManager::GetAllocator().Free(m_pSurfaceFormats);
    CMemoryManager::GetAllocator().Free(m_pSurfacePresentModes);   
    CMemoryManager::GetAllocator().Free(m_pRenderPasses);   
}

void DeviceVK::CreateGraphicsPipeline(const DeviceState* pDeviceState, VkPipeline* pPipeline, const ResourceContext* pResourceContext)
{
    VkPipelineShaderStageCreateInfo shaderStages[] {{}, {}};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = pResourceContext->m_Shaders.GetShaderModule(pDeviceState->GetShader(ShaderStageVertex), ShaderStageVertex);
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = pResourceContext->m_Shaders.GetShaderModule(pDeviceState->GetShader(ShaderStageFragment), ShaderStageFragment);
    shaderStages[1].pName = "main";

    VkDynamicState dynamicStates[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(sizeof(dynamicStates) / sizeof(VkDynamicState));
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
   
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = pDeviceState->GetCullMode();
    rasterizer.frontFace = pDeviceState->GetFrontFace();

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = pDeviceState->GetColorBlend();

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
       
    VkPipelineLayout pipelineLayout;
    VkResult res = vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, &m_AllocationCallbacks, &pipelineLayout);
    Assert(res == VK_SUCCESS);
       
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &pResourceContext->m_VertexBuffers.GetVertexInputCreateInfo(pDeviceState->GetVertexBuffer());
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = m_pRenderPasses[m_CurrentRenderPass];
    pipelineInfo.subpass = 0;

    res = vkCreateGraphicsPipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, &m_AllocationCallbacks, pPipeline);    
    Assert(res == VK_SUCCESS);
}

uint32_t DeviceVK::BeginFrame()
{
    m_CurrFrame = m_DeviceFrameCount % scm_Frames;
    vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFence[m_CurrFrame], VK_TRUE, UINT64_MAX);
    if (m_StagingBufferOffsets[m_CurrFrame] != -1)
    {
        m_StagingRingBuffer.Free(m_StagingBufferOffsets[m_CurrFrame]);  // all uploads have completed up to this offset.
        m_StagingBufferOffsets[m_CurrFrame] = -1;
    }    
    uint32_t imageIndex;
    VkResult res = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore[m_CurrFrame], VK_NULL_HANDLE, &imageIndex);
    if (res == VK_ERROR_OUT_OF_DATE_KHR)
    {                
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Win32Surface, &m_SurfaceCapabilities);
        // Handle minimize
        if ((m_SurfaceCapabilities.currentExtent.height == 0) || (m_SurfaceCapabilities.currentExtent.width == 0))
            return -1;
        DisposeSwapChain();
        CreateSwapChain(m_Win32Surface, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
        return -1;
    }
    vkResetFences(m_LogicalDevice, 1, &m_InFlightFence[m_CurrFrame]);
    vkResetCommandBuffer(m_CommandBuffer[m_CurrFrame], 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    res = vkBeginCommandBuffer(m_CommandBuffer[m_CurrFrame], &beginInfo);
    Assert(res == VK_SUCCESS);

    if (m_StagingBufferCopyCount > 0)
    {
        // Perform all the host to local memory buffer copies
        for (uint32_t i = 0; i < m_StagingBufferCopyCount; i++)
        {
            vkCmdCopyBuffer(m_CommandBuffer[m_CurrFrame], m_StagingBuffer, m_pStagingBufferCopies[i].m_DstBuffer, 1, &m_pStagingBufferCopies[i].m_BufferCopy);
        }
        m_StagingBufferCopyCount = 0;
        m_StagingBufferOffsets[m_CurrFrame] = m_CurrentStagingOffset;   // when the fence for current frame gets set we will mark ring buffer memory up to this point as available
    }
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_pRenderPasses[m_CurrentRenderPass];
    renderPassInfo.framebuffer = m_pSwapChainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_SurfaceCapabilities.currentExtent;
    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(m_CommandBuffer[m_CurrFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_SurfaceCapabilities.currentExtent.width);
    viewport.height = static_cast<float>(m_SurfaceCapabilities.currentExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_CommandBuffer[m_CurrFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_SurfaceCapabilities.currentExtent;
    vkCmdSetScissor(m_CommandBuffer[m_CurrFrame], 0, 1, &scissor);
    return imageIndex;
}

void DeviceVK::EndFrame(uint32_t imageIndex)
{
    vkCmdEndRenderPass(m_CommandBuffer[m_CurrFrame]);
    VkResult res = vkEndCommandBuffer(m_CommandBuffer[m_CurrFrame]);
    Assert(res == VK_SUCCESS);
    VkSubmitInfo submitInfo{};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore[m_CurrFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer[m_CurrFrame];
    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore[m_CurrFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    res = vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence[m_CurrFrame]);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { m_SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    m_DeviceFrameCount++;
    res = vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);
    if ((res == VK_ERROR_OUT_OF_DATE_KHR) || (res == VK_SUBOPTIMAL_KHR))
    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Win32Surface, &m_SurfaceCapabilities);
        // Handle minimize
        if ((m_SurfaceCapabilities.currentExtent.height == 0) || (m_SurfaceCapabilities.currentExtent.width == 0))
            return;
        DisposeSwapChain();
        CreateSwapChain(m_Win32Surface, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    }
}

void DeviceVK::BindPipeline(VkPipeline pipeline)
{    
    vkCmdBindPipeline(m_CommandBuffer[m_CurrFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void DeviceVK::DrawVertexBuffer(VkBuffer buffer, VkDeviceSize offset, uint32_t numVerts)
{
    vkCmdBindVertexBuffers(m_CommandBuffer[m_CurrFrame], 0, 1, &buffer, &offset);
    vkCmdDraw((m_CommandBuffer[m_CurrFrame]), numVerts, 1, 0, 0);
}

void DeviceVK::DrawIndexedVertexBuffer(VkBuffer vertexBuffer, VkDeviceSize vertexOffset, VkBuffer indexBuffer, VkDeviceSize indexOffset, uint32_t numIndices)
{
    vkCmdBindVertexBuffers(m_CommandBuffer[m_CurrFrame], 0, 1, &vertexBuffer, &vertexOffset);
    vkCmdBindIndexBuffer(m_CommandBuffer[m_CurrFrame], indexBuffer, indexOffset, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(m_CommandBuffer[m_CurrFrame], numIndices, 1, 0, 0, 0);
}

void DeviceVK::DrawVertexBuffer(const ResourceContext* pResourceContext, uint32_t vertexBuffer, uint32_t indexBuffer)
{
    VkBuffer buffer;
    uint32_t numVerts;
    pResourceContext->m_VertexBuffers.GetVertexBuffer(buffer, numVerts, vertexBuffer);
    if (indexBuffer == -1)
    {
        DrawVertexBuffer(buffer, 0, numVerts);
    }
    else
    {
        VkBuffer iBuffer;
        uint32_t numIndices;
        pResourceContext->m_IndexBuffers.GetIndexBuffer(iBuffer, numIndices, indexBuffer);
        DrawIndexedVertexBuffer(buffer, 0, iBuffer, 0, numIndices);
    }
}

void DeviceVK::WaitTillIdle()
{
    vkDeviceWaitIdle(m_LogicalDevice);
}

void DeviceVK::SetVertexInput()
{
    
}

void DeviceVK::SetInputAssembly(VkPrimitiveTopology topology, uint32_t restartEnable)
{

}

void DeviceVK::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{

}

void DeviceVK::SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{

}

void DeviceVK::SetCullMode(VkCullModeFlagBits cullMode, VkFrontFace frontFace)
{

}

void DeviceVK::SetMultisampling()
{

}

void DeviceVK::SetDepthTesting()
{

}

void DeviceVK::SetStencilTesting()
{

}

void DeviceVK::SetAttachmentBlendState(uint32_t attachmentIndex)
{

}

void DeviceVK::GetSurfaceExtent(DeviceState& deviceState)
{
    deviceState.SetWidth(m_SurfaceCapabilities.currentExtent.width);
    deviceState.SetHeight(m_SurfaceCapabilities.currentExtent.height);
}

VkShaderModule DeviceVK::CreateShaderModule(const char* code, uint32_t codeSize)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

    VkShaderModule shaderModule;
    VkResult res = vkCreateShaderModule(m_LogicalDevice, &createInfo, &m_AllocationCallbacks, &shaderModule);
    Assert(res == VK_SUCCESS);
    return shaderModule;
}

void DeviceVK::DestroyShaderModule(VkShaderModule shaderModule)
{
    vkDestroyShaderModule(m_LogicalDevice, shaderModule, &m_AllocationCallbacks);
}

void DeviceVK::CreateVertexBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, uint32_t size, const void* pData)
{
    size = ALIGN(size, 4);  // ringbuffer allocations should be 4 byte aligned for this
    // create the buffer and its memory 
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateLocalBuffer(buffer, bufferMemory, bufferInfo, size, pData);   
}

void DeviceVK::DestroyVertexBuffer(VertexBuffersVK::VertexBuffer* pBuffer)
{
    vkDestroyBuffer(m_LogicalDevice, pBuffer->m_VertexBuffer, &m_AllocationCallbacks);
    vkFreeMemory(m_LogicalDevice, pBuffer->m_BufferMemory, &m_AllocationCallbacks);
}

void DeviceVK::CreateIndexBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, uint32_t size, const void* pData)
{
    size = ALIGN(size, 4);  // ringbuffer allocations should be 4 byte aligned for this
    // create the buffer and its memory 
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateLocalBuffer(buffer, bufferMemory, bufferInfo, size, pData);
}

void DeviceVK::DestroyIndexBuffer(IndexBuffersVK::IndexBuffer* pBuffer)
{
    vkDestroyBuffer(m_LogicalDevice, pBuffer->m_IndexBuffer, &m_AllocationCallbacks);
    vkFreeMemory(m_LogicalDevice, pBuffer->m_BufferMemory, &m_AllocationCallbacks);
}

void DeviceVK::SetDeviceState(const DeviceState* pDeviceState)
{

}

void* DeviceVK::vkAllocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    return CMemoryManager::GetAllocator().AlignedAlloc(size, static_cast<unsigned int>(alignment));
}

void* DeviceVK::vkReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    size_t originalSize = CMemoryManager::GetAllocator().AlignedMsize(pOriginal);
    void* newAllocation = CMemoryManager::GetAllocator().AlignedAlloc(size, static_cast<unsigned int>(alignment));
    Assert(originalSize <= size);
    memcpy(newAllocation, pOriginal, originalSize);
    CMemoryManager::GetAllocator().Free(pOriginal);
    return newAllocation;
}

void DeviceVK::vkFreeFunction(void* pUserData, void* pMemory)
{
    if (pMemory == nullptr)
        return;
    CMemoryManager::GetAllocator().Free(pMemory);
}

void DeviceVK::vkInternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{

}

void DeviceVK::vkInternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{

}