#include "RenderMgr.h"
#include "../Util/GLProgramHandle.h"

IMPLEMENT_SINGLETON(RenderMgr);


RenderMgr::RenderMgr(): cameraMgr(new CameraMgr()) {
	m_camera = mat4::LookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
}


RenderMgr::~RenderMgr() {
	Exterminate();
}


void RenderMgr::Init() {
}


void RenderMgr::Render(float elapsedTime) const {


	ProgramRenderLayer programComp(m_rendersLayer.begin(), m_rendersLayer.end());

	for(const auto& programPair : programComp) {
		
		const auto& handler = *programPair.first;
		const auto& renderComp = programPair.second;

		glUseProgram(handler.Program);

		glUniform1i(handler.Uniforms.LightMode, 0);	//юс╫ц

		// Initialize various state.
		glEnableVertexAttribArray(handler.Attributes.Position);
		glEnableVertexAttribArray(handler.Attributes.Normal);


		for (const auto& render : renderComp) {
			if (render == nullptr) continue;

			render->SetMatrix(m_camera);
			render->Render(elapsedTime);

		}

		glDisableVertexAttribArray(handler.Attributes.Position);
		glDisableVertexAttribArray(handler.Attributes.Normal);
		
	}



}


void RenderMgr::Exterminate() {

	SAFE_DELETE(cameraMgr);

}
