#ifndef _RESOURCECONTEXT_H_
#define _RESOURCECONTEXT_H_

#include "..\GraphicsDevice\Vulkan\ShadersVK.h"
#include "..\GraphicsDevice\Vulkan\IndexBuffersVK.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"
#include "..\GraphicsDevice\ShaderConstants.h"

class ResourceContext
{
	public:
		ShaderConstants m_ShaderConstants;
		ShadersVK		m_Shaders;
		IndexBuffersVK	m_IndexBuffers;
		VertexBuffersVK m_VertexBuffers;
		void Initialize();
		void Dispose(DeviceVK* pDevice);
};

#endif
