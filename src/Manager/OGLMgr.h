#pragma once
#include "../OGLDef.h"
#include <string>

class OGLMgr
{
public:
	OGLMgr();
	~OGLMgr();
	void setShaderProgram(int id);
	void setBuffers();
	void setupEGLGraphics(GLuint width, GLuint height);

	static GLuint createProgram(const GLchar* vertexSource, const GLchar* fragmentSource);
	static GLuint createProgramfromFile(const GLchar* vertexPath, const GLchar* fragmentPath);
	static void AttachProgramHandle(int shaderID);
	void setVertexShader(GLchar* vertexSource);
	void setFragmentShader(GLchar* fragmentSource);

	void Render(float elapsedTime);
	void ResizeWindow(GLuint width, GLuint height);

	void releaseBuffers();

private:
	static GLuint loadShader(GLenum shaderType, const char *pSource);
	void setProjectionRatio();

private:
	GLuint m_width, m_height;
	GLfloat m_projectionRatio;

	GLuint m_depthRenderbuffer = 0;
	GLuint m_colorRenderbuffer = 0;
	GLuint m_framebuffer = 0;

	std::string m_vertexShader;
	std::string m_fragmentShader;

	int m_programId;
};

