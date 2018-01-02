#include "ResMgr.h"
#include "ShaderProgramContainer.h"
#include "SurfaceMeshContainer.h"
#include "CameraMgr.h"

IMPLEMENT_SINGLETON(ResMgr);

ResMgr::ResMgr(){

	Init();

}


ResMgr::~ResMgr()
{
	Exterminate();
}


void ResMgr::Init() {

	m_containers.push_back(new ShaderProgramContainer());
	m_containers.push_back(new SurfaceMeshContainer());

}


void ResMgr::Exterminate() {

}



GLProgramHandle* ResMgr::getShaderProgramHandle(int id) const {
	return GetObject<ShaderProgramContainer, GLProgramHandle>(id);
}


SISurface* ResMgr::GetSurfaceMesh(int id) const {
	return GetObject<SurfaceMeshContainer, SISurface>(id);
}
