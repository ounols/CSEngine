#pragma once

#include <list>
#include "Base/SContainer.h"
#include "../Component/LightComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/Render/RenderInterfaces.h"

namespace CSE {

    class LightMgr : public SContainer<LightComponent*>, public CoreBase {
    public:
        const int SHADOW_WIDTH = 1024;
        const int SHADOW_HEIGHT = 1024;
        const float SHADOW_DISTANCE = 10.f;

    public:
        explicit LightMgr();
        ~LightMgr();

        void AttachLightToShader(const GLProgramHandle* handle) const;
        void RenderShadowMap(GLProgramHandle* handle) const;

        void Init() override;

        void RegisterShadowObject(SIRender* object);
        void RemoveShadowObject(SIRender* object);

        int GetShadowCount() const {
            return m_shadowCount;
        }

    private:
        STexture* m_currentSkybox = nullptr;
        std::list<SIRender*> m_shadowObject;
        mutable int m_shadowCount = 0;
    };

}