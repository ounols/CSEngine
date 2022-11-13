#pragma once

#include "../../Util/Render/RenderInterfaces.h"

namespace CSE {

    class ForwardRenderGroup : public SIRenderGroup {
    public:
        ForwardRenderGroup();

        ~ForwardRenderGroup() override;

        void RenderAll(unsigned int framebufferId, const std::list<SIRender*>& renderList) const override;
    };

}
