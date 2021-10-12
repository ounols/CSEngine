//
// Created by ounol on 2021-08-14.
//

#pragma once

namespace CSE {

    class Settings {
    public:
        Settings();
        ~Settings();

        static const char* GetDeferredGeometryPassShaderID();
        static const char* GetDefaultForwardMaterialId();
        static const char* GetDefaultDeferredMaterialId();
        static const char* GetDefaultMainBufferShaderID();

    private:
        static const char* m_deferredGeometryPassShaderID;
        static const char* m_defaultForwardMaterialID;
        static const char* m_defaultDeferredMaterialID;
        static const char* m_defaultDeferredProxyMaterialID;
        static const char* m_defaultMainBufferShaderID;
    };
}
