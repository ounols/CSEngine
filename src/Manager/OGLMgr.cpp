#include "OGLMgr.h"
#include "../Util/GLProgramHandle.h"


OGLMgr::OGLMgr() : m_projectionRatio(-1), m_programId(-1) {
	m_vertexShader = "uniform mat4 u_projectionMatrix;\n"
        "uniform mat4 u_modelViewMatrix;\n"
        "attribute vec4 a_position;\n"
                "void main() {\n"
                "   gl_Position = u_modelViewMatrix * a_position;\n"
		//"   gl_Position = a_position;\n"
		//"   vColor = aColor;\n"
		"}\n";

	m_fragmentShader = "precision mediump float;\n"
                "void main() {\n"
		"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"    //모든 정점을 녹색으로 설정
														  //"   gl_FragColor = vColor;\n"                       //임의로 정의한 색으로 정점을 설정
		"}\n";

}


OGLMgr::~OGLMgr() {
}


void OGLMgr::setupEGLGraphics(int id = HANDLE_NULL) {

	GLProgramHandle* gProgramhandle;

	if (id == HANDLE_NULL) {
		//기본값 쉐이더 적용
		gProgramhandle = new GLProgramHandle();
		m_programId = RESMGR->getShaderProgramSize() - 1;

		gProgramhandle->Program = createProgram(m_vertexShader.c_str(), m_fragmentShader.c_str());
		if (!gProgramhandle->Program) {
			return;
		}
	}
	else {
		//커스텀 쉐이더 적용
		gProgramhandle = RESMGR->getShaderProgramHandle(id);
		m_programId = id;
	}

	//const auto m_program = gProgramhandle->Program;

	AttachProgramHandle(m_programId);
}


void OGLMgr::setupEGLGraphics(GLuint width, GLuint height) {

	//뷰포트 설정
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

		//쉐이더를 attach 합니다.
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


//렌더링 매니저로 이동
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

	////VBO 언바인드
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}
