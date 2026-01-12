#ifndef _SCENE_H_
#define _SCENE_H_

class Renderer;

class Scene
{
    public:
        void initialize(Renderer* pRenderer);
        void draw(Renderer* pRenderer);

    private:
        uint32_t m_RenderElement;
        uint32_t m_RenderElement2;
        uint32_t m_RenderElement3;
        uint32_t m_RenderElement4;
};

#endif
