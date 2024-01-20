#pragma once

#include <list>
#include "Base/SContainerList.h"
#include "../Component/LightComponent.h"
#include "../Util/Render/RenderInterfaces.h"

namespace CSE {

    class GLProgramHandle;
    class SEnvironmentMgr;

    /**
     * LightMgr class is a manager of Light components.
     * This class also manages ShadowMap for giving shadow effects to objects in the scene.
     */
    class LightMgr : public SContainerList<LightComponent*>, public CoreBase {
    public:
        const int SHADOW_WIDTH = 1024;     ///< Width of the shadow map
        const int SHADOW_HEIGHT = 1024;    ///< Height of the shadow map
        constexpr static float SHADOW_DISTANCE = 10.f;   ///< Distance of the shadow map

    public:
        /**
         * Constructor of LightMgr.
         */
        explicit LightMgr();

        /**
         * Destructor of LightMgr.
         */
        ~LightMgr() override;

        /**
         * Attach the light components to the shader.
         * @param handle Shader handle
         */
        void AttachLightToShader(const GLProgramHandle* handle) const;

        /**
         * Attach the light map to the shader.
         * @param handle Shader handle
         * @param textureLayout Layout of the texture
         */
        int AttachLightMapToShader(const GLProgramHandle* handle, int textureLayout) const;

        /**
         * Initialize the LightMgr.
         */
        void Init() override;

        /**
         * Get the shadow count.
         * @return Shadow count
         */
        int GetShadowCount() const {
            return m_shadowCount;
        }

        /**
         * Get the light map count.
         * @return Light map count
         */
        int GetLightMapCount() const {
            return m_lightMapCount;
        }

        /**
         * Refresh the shadow count.
         * @param shadowCount Shadow count to refresh. Default is -1.
         */
        void RefreshShadowCount(int shadowCount = -1) const;

        static void ExterminateGlobalSettings();

    private:
        std::list<SIRender*> m_shadowObject;   ///< Objects to be rendered as shadows
        mutable int m_shadowCount = 0;          ///< Count of shadows

        /*mutable*/ int m_lightMapCount = 3;    ///< Count of light maps

        SEnvironmentMgr* m_environmentMgr = nullptr;  ///< Environment manager for rendering shadows
    };

}