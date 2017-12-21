#pragma once
#include "../Macrodef.h"
#include "ShaderProgramContainer.h"
#include "SurfaceMeshContainer.h"

#define RESMGR ResMgr::getInstance()

class SObject;
class GLProgramHandle;
class SISurface;

class ResMgr {
private:
	~ResMgr();

public:
	DECLARE_SINGLETONE(ResMgr);

	void Init();
	void Exterminate();

	void RegisterProgram(GLProgramHandle* m_handle) const;
	void RegisterSurfaceMesh(SISurface* m_surface) const;

	GLProgramHandle* getShaderProgramHandle(int id) const;
	SISurface* GetSurfaceMesh(int id) const;


	int getShaderProgramSize() const {
		return m_programContainer->getSize();
	}


	bool isProgramEmpty() const {
		return m_programContainer->getSize() == 0;
	}


private:
	ShaderProgramContainer* m_programContainer;
	SurfaceMeshContainer* m_surfaceMeshContainer;
};
