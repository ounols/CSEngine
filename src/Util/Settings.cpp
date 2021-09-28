//
// Created by ounol on 2021-08-14.
//

#include "Settings.h"

using namespace CSE;

const char* Settings::m_deferredGeometryPassShaderID = "File:Shader/PBR/PBR-Geometry-Pass.shader";
const char* Settings::m_defaultForwardMaterialID = "File:Material/DefaultPBR.mat";
const char* Settings::m_defaultDeferredMaterialID = "File:Material/Default.mat";
const char* Settings::m_defaultDeferredProxyMaterialID = "File:Material/DefaultPBR.mat";

Settings::Settings() {

}

Settings::~Settings() {

}

const char* Settings::GetDeferredGeometryPassShaderID() {
    return m_deferredGeometryPassShaderID;
}

const char* Settings::GetDefaultForwardMaterialId() {
    return m_defaultForwardMaterialID;
}

const char* Settings::GetDefaultDeferredMaterialId() {
#ifdef __ANDROID__
    return m_defaultDeferredProxyMaterialID;
#else
    return m_defaultDeferredMaterialID;
#endif
}
