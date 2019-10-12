#pragma once

#include "Base/SContainer.h"
#include "../Component/LightComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/SCubeTexture.h"

namespace CSE {

    class LightMgr : public SContainer<LightComponent*> {
    public:
    DECLARE_SINGLETONE(LightMgr);

        ~LightMgr();

        void AttachLightToShader(const GLProgramHandle* handle) const;

    private:
        SCubeTexture* m_currentSkybox;
    };

}