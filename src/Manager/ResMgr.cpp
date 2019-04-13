#include "ResMgr.h"
#include "ShaderProgramContainer.h"
#include "SurfaceMeshContainer.h"
#include "SkeletonContainer.h"
#include "SPrefabContainer.h"
#include "AnimationContainer.h"
#include "CameraMgr.h"

IMPLEMENT_SINGLETON(ResMgr);

ResMgr::ResMgr() {

    Init();

}


ResMgr::~ResMgr() {
    Exterminate();
}


void ResMgr::Init() {

    m_containers.push_back(new ShaderProgramContainer());
    m_containers.push_back(new SurfaceMeshContainer());
    m_containers.push_back(new SkeletonContainer());
    m_containers.push_back(new SPrefabContainer());
    m_containers.push_back(new AnimationContainer());

}


void ResMgr::Exterminate() {
#ifdef __ANDROID__
    SAFE_DELETE(m_assetManager);
#endif
}


GLProgramHandle* ResMgr::getShaderProgramHandle(int id) const {
    return GetObject<ShaderProgramContainer, GLProgramHandle>(id);
}


SISurface* ResMgr::GetSurfaceMesh(int id) const {
    return GetObject<SurfaceMeshContainer, SISurface>(id);
}


#ifdef __ANDROID__

void ResMgr::SetAssetManager(AAssetManager* obj) {
    m_assetManager = obj;
}


AAssetManager * ResMgr::GetAssetManager() {
    return m_assetManager;
}

void ResMgr::SetEnv(JNIEnv *obj) {
    m_env = obj;
}

JNIEnv *ResMgr::GetEnv() {
    return m_env;
}

#endif
