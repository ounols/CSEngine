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

	void setVertexShader(GLchar* vertexSource);
	void setFragmentShader(GLchar* fragmentSource);

	void Render(float elapsedTime);


private:
	static GLuint createProgram(const GLchar* vertexSource, const GLchar* fragmentSource);
	static GLuint loadShader(GLenum shaderType, const char *pSource);

private:
	GLuint m_width, m_height;

	std::string m_vertexShader;
	std::string m_fragmentShader;

	GLuint m_program;
	GLuint m_vPositionHandle;
	GLuint m_aColorHandle;
};

