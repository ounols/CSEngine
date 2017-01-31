#include "MainProc.h"
#include "../Macrodef.h"


MainProc::MainProc()
{
	m_oglMgr = new OGLMgr();
}


MainProc::~MainProc()
{
	Exterminate();
}


void MainProc::Init(GLuint width, GLuint height) {

	m_oglMgr->setupEGLGraphics(width, height);

}


void MainProc::Update(float elapsedTime) {
}


void MainProc::Render(float elapsedTime) {

	m_oglMgr->Render(elapsedTime);

}


void MainProc::Exterminate() {

	SAFE_DELETE(m_oglMgr);

}
