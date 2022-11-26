#pragma once

#include <unordered_map>
#include "../../Util/Render/SRenderGroup.h"

namespace CSE {

    class LightMgr;

    class ForwardRenderGroup : public SRenderGroup {
    private:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::unordered_map<GLProgramHandle*, RenderInterfaces> ProgramRenderLayer;
        typedef std::unordered_map<short, ProgramRenderLayer> OrderRenderLayer;
    public:
        explicit ForwardRenderGroup(const RenderMgr& renderMgr);

        ~ForwardRenderGroup() override;

        void RegisterObject(SIRender* object) override;

        void RemoveObjects(SIRender* object) override;

        /**
         * Render the objects to be forward-rendered.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void RenderAll(const CameraBase& camera) const override;

        void Exterminate() override;

    private:
        OrderRenderLayer m_rendersLayer;
        LightMgr* m_lightMgr = nullptr;
    };

}
