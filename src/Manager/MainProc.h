#pragma once

#include "../OGLDef.h"
#include "OGLMgr.h"

class MainProc
{
public:
	MainProc();
	~MainProc();

	void Init(GLuint width, GLuint height);
	void Update(float elapsedTime);
	void Render(float elapsedTime);
	void Exterminate();

private:
	OGLMgr* m_oglMgr = nullptr;

};

