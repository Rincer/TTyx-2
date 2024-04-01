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
};

#endif
