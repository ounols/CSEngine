#pragma once

#include <list>
#include "Base/SContainer.h"
#include "../Component/LightComponent.h"
#include "../Util/Render/RenderInterfaces.h"

namespace CSE {

    class GLProgramHandle;
    class SEnvironmentMgr;

    class LightMgr : public SContainer<LightComponent*>, public CoreBase {
    public:
        const int SHADOW_WIDTH = 1024;
        const int SHADOW_HEIGHT = 1024;
        constexpr static float SHADOW_DISTANCE = 10.f;

    public:
        explicit LightMgr();
        ~LightMgr();

        void AttachLightToShader(const GLProgramHandle* handle) const;
        void AttachLightMapToShader(const GLProgramHandle* handle, int textureLayout) const;

        void Init() override;

        void RegisterShadowObject(SIRender* object);
        void RemoveShadowObject(SIRender* object);

        int GetShadowCount() const {
            return m_shadowCount;
        }

        int GetLightMapCount() const {
            return m_lightMapCount;
        }

        void RefreshShadowCount(int shadowCount = -1) const;
        const std::list<SIRender*>& GetShadowObject() const;
        GLProgramHandle* GetShadowHandle() const;

    private:
        std::list<SIRender*> m_shadowObject;
        mutable int m_shadowCount = 0;
        GLProgramHandle* m_shadowHandle = nullptr;

        /*mutable*/ int m_lightMapCount = 3;

        SEnvironmentMgr* m_environmentMgr = nullptr;
    };

}