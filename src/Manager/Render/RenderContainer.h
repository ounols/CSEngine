#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include "../../Util/Render/SRenderGroup.h"


namespace CSE {

    class SFrameBuffer;
    class SGBuffer;
    class SRenderGroup;

    class RenderContainer {
    public:
        enum RenderGroupMode {
            FORWARD = 0,
            DEFERRED = 1,
            DEPTH_ONLY = 2,
        };
    public:
        RenderContainer();

        ~RenderContainer();
    public:
        void Register(SIRender* object, RenderGroupMode groupMode);
        void Remove(SIRender* object, RenderGroupMode groupFlag);

    protected:
        virtual void Exterminate();

    protected:
        SRenderGroup* m_forwardRenderGroup = nullptr;
        SRenderGroup* m_deferredRenderGroup = nullptr;
        SRenderGroup* m_depthOnlyRenderGroup = nullptr;

        unsigned int* m_width = nullptr;
        unsigned int* m_height = nullptr;
    };

}