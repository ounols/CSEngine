#pragma once
#include "../Macrodef.h"
#include "../SObject.h"
#include "ShaderProgramContainer.h"

#define RESMGR ResMgr::getInstance()

class ShaderProgramContainer;
class ResMgr
{
private:
	ResMgr();
	~ResMgr();

public:
	SINGLETONE(ResMgr);

	void Init();
	void Exterminate() const;

	GLProgramHandle* getShaderProgramHandle(int id) const;

private:
	static void deleteObject(SObject* object);

private:
	ShaderProgramContainer* m_programContainer;
};
