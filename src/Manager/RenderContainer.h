#pragma once
#include <vector>
#include <list>
#include <map>
#include "../Util/Render/RenderInterfaces.h"


namespace CSE {

    class SFrameBuffer;

    class RenderContainer {
    public:
        RenderContainer();

        ~RenderContainer();

    public:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::map<GLProgramHandle*, RenderInterfaces> ProgramRenderLayer;
        typedef std::map<short, ProgramRenderLayer> OrderRenderLayer;

    public:
        void Register(SIRender* object);
        void RegisterDeferred(SIRender* object);

        void Remove(SIRender* object);
        void RemoveDeferred(SIRender* object);

    protected:
        OrderRenderLayer m_rendersLayer;
        std::list<SIRender*> m_renderDeferredLayer;
    };

}