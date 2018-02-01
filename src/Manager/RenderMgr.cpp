#include "RenderMgr.h"
#include "../Util/GLProgramHandle.h"
#include "LightMgr.h"

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

	const CameraComponent* cameraComponent = CameraMgr::getInstance()->GetCurrentCamera();
	mat4 camera = (cameraComponent != nullptr) ? 
		cameraComponent->GetMatrix() : m_NoneCamera;

	ProgramRenderLayer programComp(m_rendersLayer.begin(), m_rendersLayer.end());

	for(const auto& programPair : programComp) {
		
		const auto& handler = *programPair.first;
		const auto& renderComp = programPair.second;

		if(programPair.first == nullptr) continue;

		glUseProgram(handler.Program);

		glUniform1i(handler.Uniforms.LightMode, 0);	//юс╫ц
		LightMgr::getInstance()->AttachLightToShader(&handler);

		// Initialize various state.
		glEnableVertexAttribArray(handler.Attributes.Position);
		glEnableVertexAttribArray(handler.Attributes.Normal);


		for (const auto& render : renderComp) {
			if (render == nullptr) continue;
			if(!render->isRenderActive) continue;

			render->SetMatrix(camera);
			render->Render(elapsedTime);

		}

		glDisableVertexAttribArray(handler.Attributes.Position);
		glDisableVertexAttribArray(handler.Attributes.Normal);
		
	}



}


void RenderMgr::Exterminate() {
	m_rendersLayer.clear();

}
