#pragma once
#include <vector>
#include "../Util/Render/RenderInterfaces.h"
#include <map>


namespace CSE {

    class RenderContainer {
    public:
        RenderContainer();

        ~RenderContainer();

    public:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::map<GLProgramHandle*, RenderInterfaces> ProgramRenderLayer;

    public:
        void Register(SIRender* object);

        void Remove(SIRender* object);

    protected:
        ProgramRenderLayer m_rendersLayer;
    };

}