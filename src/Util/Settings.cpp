//
// Created by ounol on 2021-08-14.
//

#include "Settings.h"

using namespace CSE;

const char* Settings::m_deferredGeometryPassShaderID = "File:Shader/PBR/PBR-Geometry-Pass.shader";
const char* Settings::m_defaultFowardMaterialID = "File:Material/DefaultPBR.mat";

Settings::Settings() {

}

Settings::~Settings() {

}

const char* Settings::GetDeferredGeometryPassShaderID() {
    return m_deferredGeometryPassShaderID;
}

const char* Settings::GetDefaultFowardMaterialId() {
    return m_defaultFowardMaterialID;
}
