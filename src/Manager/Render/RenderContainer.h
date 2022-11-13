#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include "../../Util/Render/RenderInterfaces.h"


namespace CSE {

    class SFrameBuffer;
    class SGBuffer;

    class RenderContainer {
    public:
        RenderContainer();

        ~RenderContainer();

    protected:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::unordered_map<GLProgramHandle*, SGBuffer*> GBufferLayer;
        typedef std::unordered_map<GLProgramHandle*, RenderInterfaces> ProgramRenderLayer;
        typedef std::unordered_map<short, ProgramRenderLayer> OrderRenderLayer;

    public:
        void Register(SIRender* object);
        void RegisterDeferred(SIRender* object, const SMaterial* material);

        void Remove(SIRender* object);
        void RemoveDeferred(SIRender* object, const SMaterial* material);

    protected:
        virtual void Exterminate();

    protected:
        OrderRenderLayer m_rendersLayer;
        GBufferLayer m_gbufferLayer;

        unsigned int* m_width = nullptr;
        unsigned int* m_height = nullptr;
    };

}