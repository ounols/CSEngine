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

}


void ResMgr::Exterminate() {


}


void ResMgr::RegisterProgram(GLProgramHandle* m_handle) const {
	m_programContainer->RegisterProgram(m_handle);
}


GLProgramHandle* ResMgr::getShaderProgramHandle(int id) const {
	return m_programContainer->getProgramHandle(id);
}

