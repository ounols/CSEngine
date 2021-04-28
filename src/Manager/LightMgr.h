#pragma once

#include <list>
#include "Base/SContainer.h"
#include "../Component/LightComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/SCubeTexture.h"
#include "../Util/Render/RenderInterfaces.h"

namespace CSE {

    class LightMgr : public SContainer<LightComponent*>, public CoreBase {
    private:
        const int SHADOW_WIDTH = 1024;
        const int SHADOW_HEIGHT = 1024;

    public:
        explicit LightMgr();
        ~LightMgr();

        void AttachLightToShader(const GLProgramHandle* handle) const;
        void RenderShadowMap() const;

        void Init() override;

        void RegisterShadowObject(SIRender* object);
        void RemoveShadowObject(SIRender* object);

    private:
        SCubeTexture* m_currentSkybox = nullptr;
        std::list<SIRender*> m_shadowObject;
    };

}