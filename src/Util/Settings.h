//
// Created by ounol on 2021-08-14.
//

#pragma once

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

        //Assets
        static bool IsAssetsPacked() {
            return m_assetsPacked;
        }

    private:
        // Renderer
        static const char* m_deferredGeometryPassShaderID;
        static const char* m_defaultForwardMaterialID;
        static const char* m_defaultDeferredMaterialID;
        static const char* m_defaultDeferredProxyMaterialID;
        static const char* m_defaultMainBufferShaderID;

        // Assets
        static const bool m_assetsPacked;
    };
}
