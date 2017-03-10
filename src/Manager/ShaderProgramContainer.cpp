#include "ShaderProgramContainer.h"
#include "../Macrodef.h"
#include "../Util/GLProgramHandle.h"


ShaderProgramContainer::ShaderProgramContainer()
{
}


ShaderProgramContainer::~ShaderProgramContainer()
{
	ShaderProgramContainer::Exterminate();
}


void ShaderProgramContainer::RegisterProgram(GLProgramHandle* m_handle) {

	m_programHandles.push_back(m_handle);
	m_size++;

}


void ShaderProgramContainer::Exterminate() {

}
