#include "ShaderProgramContainer.h"
#include "../Macrodef.h"

ShaderProgramContainer::ShaderProgramContainer()
{
}


ShaderProgramContainer::~ShaderProgramContainer()
{
	ShaderProgramContainer::Exterminate();
}


void ShaderProgramContainer::RegisterProgram(GLProgramHandle* m_handle) {

	m_programHandles.push_back(m_handle);

}


void ShaderProgramContainer::Exterminate() {

	for (const auto& handle : m_programHandles) {
		if (handle == nullptr)	continue;

		//handle->Exterminate();
		delete handle;
	}

}
