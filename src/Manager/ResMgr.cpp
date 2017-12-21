#include "ResMgr.h"

IMPLEMENT_SINGLETON(ResMgr);

ResMgr::ResMgr(){

	Init();

}


ResMgr::~ResMgr()
{
	Exterminate();
}


void ResMgr::Init() {

	m_programContainer = new ShaderProgramContainer();
	m_surfaceMeshContainer = new SurfaceMeshContainer();

}


void ResMgr::Exterminate() {


}


void ResMgr::RegisterProgram(GLProgramHandle* m_handle) const {
	m_programContainer->RegisterProgram(m_handle);
}


void ResMgr::RegisterSurfaceMesh(SISurface* m_surface) const {
	m_surfaceMeshContainer->RegisterProgram(m_surface);
}


GLProgramHandle* ResMgr::getShaderProgramHandle(int id) const {
	return m_programContainer->getProgramHandle(id);
}


SISurface* ResMgr::GetSurfaceMesh(int id) const {
	return m_surfaceMeshContainer->GetSurfaceMesh(id);
}
