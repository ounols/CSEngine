#pragma once

#include "../../Util/Render/SRenderGroup.h"
#include <list>

namespace CSE {

    class DepthOnlyRenderGroup : public SRenderGroup {
    public:
        explicit DepthOnlyRenderGroup(const RenderMgr& renderMgr);

        void RegisterObject(SIRender* object) override;

        void RemoveObjects(SIRender* object) override;

        /**
         * Rendering for a depth buffer to use for shadows. The render target depends on the shadow settings.
         * @param camera The light(camera) you want to render.
         */
        void RenderAll(const CameraBase& camera) const override;

        void Exterminate() override;

    private:
        std::list<SIRender*> m_depthObjects;
        GLProgramHandle* m_shadowHandle = nullptr;
    };
}