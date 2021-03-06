#include "../Util/Matrix.h"
#include "OGLMgr.h"
#include "../Util/GLProgramHandle.h"
#include "CameraMgr.h"
#include "EngineCore.h"
#include "../Util/Render/SEnvironmentMgr.h"

#ifdef __linux__

#include <iostream>

#endif

using namespace CSE;

OGLMgr::OGLMgr() : m_projectionRatio(-1) {
}

void OGLMgr::Init() {
	setupEGLGraphics();
}

OGLMgr::~OGLMgr() {
	releaseBuffers();
}

void OGLMgr::setBuffers() {
	//렌더버퍼 생성
	glGenRenderbuffers(1, &m_colorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

	//깊이버퍼 생성 및 바인딩(옮김)
	//glGenRenderbuffers(1, &m_depthRenderbuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);

	//프레임버퍼 생성
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

	// 렌더버퍼 바인딩
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

	glEnable(GL_DEPTH_TEST);
	glClearDepthf(1.0f);

#ifndef PLATFORM_IOS
	//프레임버퍼 바인딩
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

void OGLMgr::setupEGLGraphics() {
	setBuffers();
	setProjectionRatio(0, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glDepthRangef(0.0,1.0);
}

void OGLMgr::setProjectionRatio(int width, int height) {
	m_projectionRatio = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
	CORE->GetCore(CameraMgr)->SetProjectionRatio(m_projectionRatio);
}

void OGLMgr::Render() const {
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	////VBO 언바인딩
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 유효하지 않은 값이라서 주석처리함.
//	glEnable(GL_TEXTURE_2D);
}

void OGLMgr::ResizeWindow(GLuint width, GLuint height) {
	if (m_depthRenderbuffer) {
		glDeleteRenderbuffers(1, &m_depthRenderbuffer);
		m_depthRenderbuffer = 0;
	}

	// Create the depth buffer.
	glGenRenderbuffers(1, &m_depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

	// Protect against a divide by zero
	if (height == 0) {
		height = 1;
	}

	// Setup our viewport.
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	SEnvironmentMgr::SetWidth(width);
	SEnvironmentMgr::SetHeight(height);

	setProjectionRatio(width, height);
}

void OGLMgr::releaseBuffers() {
	glDeleteRenderbuffers(1, &m_colorRenderbuffer);
	m_colorRenderbuffer = 0;

	if (m_depthRenderbuffer) {
		glDeleteRenderbuffers(1, &m_depthRenderbuffer);
		m_depthRenderbuffer = 0;
	}

	glDeleteFramebuffers(1, &m_framebuffer);
	m_framebuffer = 0;
}
