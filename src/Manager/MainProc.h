#pragma once

#include "../OGLDef.h"
#include "OGLMgr.h"
#include "ScriptMgr.h"

class MainProc
{
public:
	MainProc();
	~MainProc();

	void Init(GLuint width, GLuint height);
    void ResizeWindow(GLuint width, GLuint height) const;
	void Update(float elapsedTime);
	void Render(float elapsedTime) const;
	void Exterminate();

private:
	OGLMgr* m_oglMgr = nullptr;
	ScriptMgr* m_scriptMgr = nullptr;

};

