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
        static const char* GetDefaultFowardMaterialId();

    private:
        static const char* m_deferredGeometryPassShaderID;
        static const char* m_defaultFowardMaterialID;
    };
}
