#ifndef _RESOURCECONTEXT_H_
#define _RESOURCECONTEXT_H_

#include "..\GraphicsDevice\Vulkan\ShadersVK.h"
#include "..\GraphicsDevice\Vulkan\IndexBuffersVK.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"

class ResourceContext
{
	public:
		ShadersVK		m_Shaders;
		IndexBuffersVK	m_IndexBuffers;
		VertexBuffersVK m_VertexBuffers;
		void Initialize();
		void Dispose(DeviceVK* pDevice);
};

#endif
