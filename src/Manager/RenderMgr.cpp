#include "RenderMgr.h"
#include "../Util/GLProgramHandle.h"
#include "LightMgr.h"
// #include <iostream>

IMPLEMENT_SINGLETON(RenderMgr);


RenderMgr::RenderMgr() {
	m_NoneCamera = mat4::LookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
}


RenderMgr::~RenderMgr() {
	Exterminate();
}


void RenderMgr::Init() {
}


void RenderMgr::Render(float elapsedTime) const {

	CameraComponent* cameraComponent = CameraMgr::getInstance()->GetCurrentCamera();
	mat4 camera = (cameraComponent != nullptr) ? 
		cameraComponent->GetCameraMatrix() : m_NoneCamera;
	mat4 projection = (cameraComponent != nullptr) ?
		cameraComponent->GetProjectionMatrix() : mat4::Identity();

	ProgramRenderLayer programComp(m_rendersLayer.begin(), m_rendersLayer.end());

	for(const auto& programPair : programComp) {
		
		const auto& handler = *programPair.first;
		const auto& renderComp = programPair.second;

		if(programPair.first == nullptr) continue;
        if(renderComp.size() <= 0) continue;

		glUseProgram(handler.Program);
		

		//Attach Light
		LightMgr::getInstance()->AttachLightToShader(&handler);

		


		for (const auto& render : renderComp) {
			if (render == nullptr) continue;
			if(!render->isRenderActive) continue;

			// Initialize various state.
			glEnableVertexAttribArray(handler.Attributes.Position);
			glEnableVertexAttribArray(handler.Attributes.Normal);
			glEnableVertexAttribArray(handler.Attributes.TextureCoord);

			render->SetMatrix(camera, projection);
			render->Render(elapsedTime);

			glDisableVertexAttribArray(handler.Attributes.Position);
			glDisableVertexAttribArray(handler.Attributes.Normal);
			glDisableVertexAttribArray(handler.Attributes.TextureCoord);

		}


		
	}

		////VBO 언바인딩
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void RenderMgr::Exterminate() {
	m_rendersLayer.clear();

}
