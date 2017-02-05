#pragma once
#include "../SObject.h"
#include "../Util/GLProgramHandle.h"

#include <vector>


class ShaderProgramContainer :
	public SObject
{

public:
	ShaderProgramContainer();
	~ShaderProgramContainer();

	void RegisterProgram(GLProgramHandle* m_handle);
	void Exterminate() override;

	GLProgramHandle* getProgramHandle(int id) {
		return m_programHandles[id];
	}

private:
	std::vector<GLProgramHandle*> m_programHandles;
};
