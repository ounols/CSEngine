#pragma once
#include "../Macrodef.h"
#include "ShaderProgramContainer.h"

#define RESMGR ResMgr::getInstance()

class SObject;
class GLProgramHandle;

class ResMgr {
private:
	~ResMgr();

public:
	DECLARE_SINGLETONE(ResMgr);

	void Init();
	void Exterminate();

	void RegisterProgram(GLProgramHandle* m_handle) const;
	GLProgramHandle* getShaderProgramHandle(int id) const;


	int getShaderProgramSize() const {
		return m_programContainer->getSize();
	}


	bool isProgramEmpty() const {
		return m_programContainer->getSize() == 0;
	}


private:
	ShaderProgramContainer* m_programContainer;
};
