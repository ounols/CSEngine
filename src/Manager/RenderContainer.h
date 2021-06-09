#pragma once
#include <vector>
#include "../Util/Render/RenderInterfaces.h"
#include <map>


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
        void RegisterFrameBuffer(SFrameBuffer* buffer);

        void Remove(SIRender* object);
        void RemoveFrameBuffer(SFrameBuffer* buffer);

    protected:
        OrderRenderLayer m_rendersLayer;
        std::vector<SFrameBuffer*> m_frameBuffer;
    };

}