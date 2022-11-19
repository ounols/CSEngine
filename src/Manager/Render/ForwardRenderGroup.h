#pragma once

#include <unordered_map>
#include "../../Util/Render/RenderInterfaces.h"

namespace CSE {

    class ForwardRenderGroup : public SIRenderGroup {
    private:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::unordered_map<GLProgramHandle*, RenderInterfaces> ProgramRenderLayer;
        typedef std::unordered_map<short, ProgramRenderLayer> OrderRenderLayer;
    public:
        explicit ForwardRenderGroup(const RenderMgr& renderMgr);

        ~ForwardRenderGroup() override;

        void RegisterObject(SIRender* object) override;

        void RemoveObjects(SIRender* object) override;

        void RenderAll(const CameraBase& camera) const override;

        void Exterminate() override;

    private:
        OrderRenderLayer m_rendersLayer;
    };

}
