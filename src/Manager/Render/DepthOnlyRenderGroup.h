#pragma once

#include "../../Util/Render/RenderInterfaces.h"
#include <list>

namespace CSE {

    class DepthOnlyRenderGroup : public SIRenderGroup {
    public:
        explicit DepthOnlyRenderGroup(const RenderMgr& renderMgr);

        void RegisterObject(SIRender* object) override;

        void RemoveObjects(SIRender* object) override;

        void RenderAll(const CameraBase& camera) const override;

        void Exterminate() override;

    private:
        std::list<SIRender*> m_depthObjects;
        GLProgramHandle* m_shadowHandle = nullptr;
    };
}