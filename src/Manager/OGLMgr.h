#pragma once
#include "../OGLDef.h"
#include <string>

class OGLMgr
{
public:
	OGLMgr();
	~OGLMgr();

	void setupEGLGraphics();
	void setupEGLGraphics(GLuint width, GLuint height);

	GLuint createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) const;
	static GLuint loadShader(GLenum shaderType, const char *pSource);

	void setVertexShader(GLchar* vertexSource);
	void setFragmentShader(GLchar* fragmentSource);

private:
	GLuint m_width, m_height;

	std::string m_vertexShader;
	std::string m_fragmentShader;

	GLuint m_program;
	GLuint m_vPositionHandle;
	GLuint m_aColorHandle;
};

