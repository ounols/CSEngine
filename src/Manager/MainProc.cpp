#ifndef __CSE_EDITOR__

#include "MainProc.h"
#include "EngineCore.h"

using namespace CSE;

#define __CSE_REFLECTION_ENABLE__
#include "ReflectionMgr.h"

MainProc::MainProc() = default;


MainProc::~MainProc() {
    Exterminate();
}


void MainProc::Init(GLuint width, GLuint height) {

    CORE->Init(width, height);
}


void MainProc::Update(float elapsedTime) {
    CORE->Update(elapsedTime);
    CORE->LateUpdate(elapsedTime);
}


void MainProc::Render(float elapsedTime) const {
    CORE->Render();
}


void MainProc::Exterminate() {
    CORE->Exterminate();
    CORE->delInstance();
}

void MainProc::ResizeWindow(GLuint width, GLuint height) const {
    CORE->ResizeWindow(width, height);
}

void MainProc::SetDeviceBuffer(unsigned int id) {
    CORE->SetDeviceBuffer(id);
}

void MainProc::GenerateCores() {
    CORE->GenerateCores();
}

#endif