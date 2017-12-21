#pragma once
#include "../Macrodef.h"
#include "RenderContainer.h"
class RenderMgr : public RenderContainer {
public:
	DECLARE_SINGLETONE(RenderMgr);
	~RenderMgr();

public:
	void Init();
	void Render(float elapsedTime) const;

private:
	mat4 m_camera;	//юс╫ц
};

