#include "stdafx.h"
#include "..\Rendering\Renderer.h"
#include "..\GraphicsDevice\Vulkan\VertexBuffersVK.h"

#include "Scene.h"

struct Vertex
{
    float pos[2];
    float color[3];
};

const Vertex vertices[3] = 
{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

const Vertex vertices2[4] = 
{
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const uint16_t indices[6] = 
{
    0, 1, 2, 2, 3, 0
};

void Scene::initialize(Renderer* pRenderer)
{
    VertexBuffersVK::VertexInputState vertexInputState;
    vertexInputState.AddBinding(0, sizeof(Vertex));
    vertexInputState.AddAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
    vertexInputState.AddAttribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
    m_RenderElement = pRenderer->CreateRenderElement("Resources\\Shaders\\triangle_vert.spv", "Resources\\Shaders\\triangle_frag.spv", vertexInputState, sizeof(vertices), vertices, 3);
    m_RenderElement2 = pRenderer->CreateIndexedRenderElement("Resources\\Shaders\\triangle_vert.spv", "Resources\\Shaders\\triangle_frag.spv", vertexInputState, sizeof(vertices2), vertices2, 4,
        sizeof(indices), indices, 6);
    m_RenderElement3 = pRenderer->CreateIndexedRenderElement("Resources\\Shaders\\triangle3d_vert.spv", "Resources\\Shaders\\triangle_frag.spv", vertexInputState, sizeof(vertices2), vertices2, 4,
        sizeof(indices), indices, 6);

}

void Scene::draw(Renderer* pRenderer)
{
    //pRenderer->DrawRenderElement(m_RenderElement2);
    //pRenderer->DrawRenderElement(m_RenderElement);
    pRenderer->DrawRenderElement(m_RenderElement3);
}