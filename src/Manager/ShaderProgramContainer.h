#pragma once
#include "../SObject.h"

#include <vector>


class GLProgramHandle;

class ShaderProgramContainer : public SObject
{

public:
	ShaderProgramContainer();
	~ShaderProgramContainer();

	void RegisterProgram(GLProgramHandle* m_handle);
	void Exterminate() override;

	GLProgramHandle* getProgramHandle(int id) {
		return m_programHandles[id];
	}

	int getSize() const {
		return m_size;
	}

private:
	std::vector<GLProgramHandle*> m_programHandles;
	int m_size = 0;
};
