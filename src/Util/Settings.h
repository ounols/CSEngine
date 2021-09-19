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

    private:
        static const char* m_deferredGeometryPassShaderID;
        static const char* m_defaultForwardMaterialID;
        static const char* m_defaultDeferredMaterialID;
    };
}
