#pragma once

#include "Base/SContainer.h"
#include "../Component/LightComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/SCubeTexture.h"

namespace CSE {

    class LightMgr : public SContainer<LightComponent*>, public CoreBase {
    public:
        explicit LightMgr();
        ~LightMgr();

        void AttachLightToShader(const GLProgramHandle* handle) const;

        void Init() override;

    private:
        SCubeTexture* m_currentSkybox;
    };

}