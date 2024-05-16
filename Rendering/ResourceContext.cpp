#include "stdafx.h"
#include "..\GraphicsDevice\Vulkan\DeviceVK.h"

#include "ResourceContext.h"


void ResourceContext::Initialize()
{
	m_Shaders.Initialize();
	m_VertexBuffers.Initialize();
	m_IndexBuffers.Initialize();

}

void ResourceContext::Dispose(DeviceVK* pDevice)
{
	m_Shaders.Dispose(pDevice);
	m_VertexBuffers.Dispose(pDevice);
	m_IndexBuffers.Dispose(pDevice);
}
