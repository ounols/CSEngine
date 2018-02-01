#include "OGLMgr.h"
#include "../Util/GLProgramHandle.h"
#include "../Util/AssetsDef.h"

using namespace CSE;

OGLMgr::OGLMgr() : m_projectionRatio(-1), m_programId(-1) {

	m_vertexShader = OpenAssetsTxtFile(AssetsPath() + "Default.vert");
	m_fragmentShader = OpenAssetsTxtFile(AssetsPath() + "Default.frag");

}


OGLMgr::~OGLMgr() {
	releaseBuffers();
}


void OGLMgr::setShaderProgram(int id = HANDLE_NULL) {

	GLProgramHandle* gProgramhandle;

	if (id == HANDLE_NULL) {
		//�⺻�� ���̴� ����
		gProgramhandle = new GLProgramHandle();
		m_programId = RESMGR->GetSize<ShaderProgramContainer, GLProgramHandle>() - 1;

		gProgramhandle->Program = createProgram(m_vertexShader.c_str(), m_fragmentShader.c_str());
		if (!gProgramhandle->Program) {
			return;
		}
	}
	else {
		//Ŀ���� ���̴� ����
		gProgramhandle = RESMGR->getShaderProgramHandle(id);
		m_programId = id;
	}

	//const auto m_program = gProgramhandle->Program;

	AttachProgramHandle(m_programId);
}


void OGLMgr::setBuffers() {

	//�������� ����
	glGenRenderbuffers(1, &m_colorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

	//���̹��� ����
	glGenRenderbuffers(1, &m_depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);

	//�����ӹ��� ������Ʈ�� ����
	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER,
		m_colorRenderbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,
		m_depthRenderbuffer);

	// �������� ���� �������۸� ���ε�
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

#ifndef PLATFORM_IOS
	//�����ӹ��� ���ε�
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}


void OGLMgr::setupEGLGraphics(GLuint width, GLuint height) {

	//����Ʈ ����
	glViewport(0, 0, width, height);
	m_width = width;
	m_height = height;

	setBuffers();
	setShaderProgram();
	setPerspectiveView();

	glEnable(GL_DEPTH_TEST);
	glClearDepthf(1.0f);
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

		glDetachShader(program, vertexShader);
		glDetachShader(program, pixelShader);

	}

	glDeleteShader(vertexShader);
	glDeleteShader(pixelShader);

	return program;

}


GLuint OGLMgr::createProgramfromFile(const GLchar* vertexPath, const GLchar* fragmentPath) {

	return 0;
}


void OGLMgr::AttachProgramHandle(int shaderID) {

	GLProgramHandle* gProgramhandle;
	GLuint program;

	gProgramhandle = RESMGR->getShaderProgramHandle(shaderID);
	program = gProgramhandle->Program;

	glUseProgram(program);

	gProgramhandle->Attributes.Position = glGetAttribLocation(program, "a_position");
	gProgramhandle->Attributes.Normal = glGetAttribLocation(program, "a_normal");
	gProgramhandle->Attributes.DiffuseMaterial = glGetAttribLocation(program, "a_diffuseMaterial");
	gProgramhandle->Attributes.TextureCoord = glGetAttribLocation(program, "a_textureCoordIn");
	gProgramhandle->Uniforms.Projection = glGetUniformLocation(program, "u_projectionMatrix");
	gProgramhandle->Uniforms.Modelview = glGetUniformLocation(program, "u_modelViewMatrix");
	gProgramhandle->Uniforms.NormalMatrix = glGetUniformLocation(program, "u_normalMatrix");
	gProgramhandle->Uniforms.LightPosition = glGetUniformLocation(program, "u_lightPosition");
	gProgramhandle->Uniforms.DiffuseLight = glGetUniformLocation(program, "u_diffuseLight");
	gProgramhandle->Uniforms.AmbientLight = glGetUniformLocation(program, "u_ambientLight");
	gProgramhandle->Uniforms.SpecularLight = glGetUniformLocation(program, "u_specularLight");
	gProgramhandle->Uniforms.AmbientMaterial = glGetUniformLocation(program, "u_ambientMaterial");
	gProgramhandle->Uniforms.SpecularMaterial = glGetUniformLocation(program, "u_specularMaterial");
	gProgramhandle->Uniforms.Shininess = glGetUniformLocation(program, "u_shininess");
	gProgramhandle->Uniforms.LightMode = glGetUniformLocation(program, "u_lightMode");
	gProgramhandle->Uniforms.Interpolation_z = glGetUniformLocation(program, "u_interpolation_z");
	gProgramhandle->Uniforms.AttenuationFactor = glGetUniformLocation(program, "u_attenuationFactor");
	gProgramhandle->Uniforms.IsAttenuation = glGetUniformLocation(program, "u_computeAttenuation");
	gProgramhandle->Uniforms.LightRadius = glGetUniformLocation(program, "u_lightRadius");
	gProgramhandle->Uniforms.SpotDirection = glGetUniformLocation(program, "u_spotDirection");
	gProgramhandle->Uniforms.SpotExponent = glGetUniformLocation(program, "u_spotExponent");
	gProgramhandle->Uniforms.SpotCutOffAngle = glGetUniformLocation(program, "u_spotCutOffAngle");
	gProgramhandle->Uniforms.IsDirectional = glGetUniformLocation(program, "u_isDirectional");

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
					OutputDebugStringA(buf);
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

	////VBO ����ε�
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}


void OGLMgr::releaseBuffers() {

	glDeleteFramebuffers(1, &m_framebuffer);
	m_framebuffer = 0;
	glDeleteRenderbuffers(1, &m_colorRenderbuffer);
	m_colorRenderbuffer = 0;

	if (m_depthRenderbuffer) {
		glDeleteRenderbuffers(1, &m_depthRenderbuffer);
		m_depthRenderbuffer = 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
