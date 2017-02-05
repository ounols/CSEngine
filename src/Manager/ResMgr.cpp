#include "ResMgr.h"


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


void ResMgr::Exterminate() const {

	deleteObject(m_programContainer);

}


GLProgramHandle* ResMgr::getShaderProgramHandle(int id) const {
	return m_programContainer->getProgramHandle(id);
}


void ResMgr::deleteObject(SObject* object) {

	object->Exterminate();
	SAFE_DELETE(object);

}
