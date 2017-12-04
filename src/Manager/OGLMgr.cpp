#include "OGLMgr.h"
#include "../Util/GLProgramHandle.h"


OGLMgr::OGLMgr() : m_projectionRatio(-1), m_programId(-1) {
}


OGLMgr::~OGLMgr() {
}


void OGLMgr::setupEGLGraphics(int id = HANDLE_NULL) {

	GLProgramHandle* gProgramhandle;

	if (id == HANDLE_NULL) {
		//�⺻�� ���̴� ����
		gProgramhandle = new GLProgramHandle();
		m_programId = RESMGR->getShaderProgramSize();

		gProgramhandle->Program = createProgram(m_vertexShader.c_str(), m_fragmentShader.c_str());
		if (gProgramhandle->Program) {
			return;
		}
	}
	else {
		//Ŀ���� ���̴� ����
		gProgramhandle = RESMGR->getShaderProgramHandle(id);
		m_programId = id;
	}

	const auto m_program = gProgramhandle->Program;

	//���ؽ� ������ �ڵ��� ���̴� �������� �޾ƿ�
	gProgramhandle->Attributes.Position = static_cast<GLuint>(glGetAttribLocation(m_program, "vPosition"));

	//���ؽ� �÷� �ڵ��� ���̴� �������� �޾ƿ�
	gProgramhandle->Attributes.Color = static_cast<GLuint>(glGetAttribLocation(m_program, "aColor"));

	//�߰��ٶ�
	//...
}


void OGLMgr::setupEGLGraphics(GLuint width, GLuint height) {

	//����Ʈ ����
	glViewport(0, 0, width, height);
	m_width = width;
	m_height = height;

	setupEGLGraphics();
	setPerspectiveView();
}


GLuint OGLMgr::createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {

	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
	if (!vertexShader) {
		return 0;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
	if (!pixelShader) {
		return 0;
	}

	GLuint program = glCreateProgram();

	if (program) {

		//���̴��� attach �մϴ�.
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glLinkProgram(program);

		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {

			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

			if (bufLength) {

				auto buf = static_cast<char *>(malloc(bufLength));

				if (buf) {

					glGetProgramInfoLog(program, bufLength, NULL, buf);
					//LOGE("Could not link program:\n%s\n", buf);
					free(buf);

				}
			}

			glDeleteProgram(program);
			program = 0;

		}
	}
	return program;

}


GLuint OGLMgr::loadShader(GLenum shaderType, const char* pSource) {

	GLuint shader = glCreateShader(shaderType);

	if (shader) {

		glShaderSource(shader, 1, &pSource, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {

			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen) {

				auto buf = static_cast<char *>(malloc(infoLen));

				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					//LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
					free(buf);
				}

				glDeleteShader(shader);
				shader = 0;
			}
		}
	}

	return shader;

}


//������ �Ŵ����� �̵�
void OGLMgr::setPerspectiveView() {
	if (m_projectionRatio < 0) {
		if (m_width > m_height)
			m_projectionRatio = (GLfloat)m_width / (GLfloat)m_height;
		else
			m_projectionRatio = (GLfloat)m_height / (GLfloat)m_width;
	}


}


void OGLMgr::setVertexShader(GLchar* vertexSource) {
	m_vertexShader = vertexSource;
}


void OGLMgr::setFragmentShader(GLchar* fragmentSource) {
	m_fragmentShader = fragmentSource;
}


void OGLMgr::Render(float elapsedTime) {

	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

}
