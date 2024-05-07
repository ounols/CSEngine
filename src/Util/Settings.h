#pragma once

#define CSE_SETTINGS_RENDER_DEFERRED_SUPPORT
#define CSE_SETTINGS_RENDER_FORWARD_SUPPORT
#define CSE_SETTINGS_RENDER_SDFGI_SUPPORT

namespace CSE {
    class Settings {
    public:
        Settings();
        ~Settings();

        // Renderer
        static const char* GetDeferredGeometryPassShaderID();
        static const char* GetDefaultForwardMaterialId();
        static const char* GetDefaultDeferredMaterialId();
        static const char* GetDefaultMainBufferShaderID();
        static const char* GetDefaultBlitBufferShaderID();
        static int GetMaxLights();

        // Assets
        static bool IsAssetsPacked() {
            return m_assetsPacked;
        }

        // Animation
        static int GetMaxJoints();

    private:
        // Renderer
        static const char* m_deferredGeometryPassShaderID;
        static const char* m_defaultForwardMaterialID;
        static const char* m_defaultDeferredMaterialID;
        static const char* m_defaultDeferredProxyMaterialID;
        static const char* m_defaultMainBufferShaderID;
        static const char* m_defaultBlitBufferShaderID;
        static const int m_maxLights;

        // Assets
        static const bool m_assetsPacked;

        // Animation
        static const int m_maxJoints;
    };
}
