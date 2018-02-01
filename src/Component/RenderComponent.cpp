#include "RenderComponent.h"
#include "../OGLDef.h"
#include "TransformComponent.h"
#include "../Util/GLProgramHandle.h"
#include "../Manager/RenderMgr.h"


RenderComponent::RenderComponent() {

}


RenderComponent::~RenderComponent() {}


void RenderComponent::Exterminate() {
}


void RenderComponent::Init() {

	SetVectorInfomation();
	m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();
	RenderMgr::getInstance()->Register(this);
	isRenderActive = isEnable;
}


void RenderComponent::Tick(float elapsedTime) {

	if(m_mesh == nullptr) {
		m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();
	}

	isRenderActive = isEnable;

}


void RenderComponent::SetMatrix(mat4 camera) {

	//model-view
	mat4 scale = mat4::Scale(m_scale->x, m_scale->y, m_scale->z);
	mat4 translation = mat4::Translate(m_position->x, m_position->y, m_position->z);
	//юс╫ц rotation
	mat4 rotationY = mat4::RotateY(m_rotation->y);
	//mat4 rotation = 
	mat4 modelView = scale * /*rotation*/ rotationY * translation * camera;
	glUniformMatrix4fv(handler->Uniforms.Modelview, 1, 0, modelView.Pointer());


	//normal matrix
	glUniformMatrix3fv(handler->Uniforms.NormalMatrix, 1, 0, modelView.ToMat3().Pointer());


	//projection transform


	//other
	glVertexAttrib4f(handler->Attributes.DiffuseMaterial, 0.75f, 0.75f, 0.75f, 1);

}


void RenderComponent::Render(float elapsedTime) {

	if (m_mesh == nullptr) return;

	int stride = 2 * sizeof(vec3);
	const auto& drawable_id = m_mesh->m_meshId;
	const GLvoid* offset = (const GLvoid*) sizeof(vec3);
	GLint position = handler->Attributes.Position;
	GLint normal = handler->Attributes.Normal;

	if(drawable_id.m_indexSize < 0) {

		glBindBuffer(GL_ARRAY_BUFFER, drawable_id.m_vertexBuffer);
		glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);

		glDrawArrays(GL_TRIANGLES, 0, drawable_id.m_vertexSize);

	}else {
		
		glBindBuffer(GL_ARRAY_BUFFER, drawable_id.m_vertexBuffer);
		glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable_id.m_indexBuffer);

		glDrawElements(GL_TRIANGLES, drawable_id.m_indexSize, GL_UNSIGNED_SHORT, 0);

	}

	glDisableVertexAttribArray(handler->Attributes.Position);
	glDisableVertexAttribArray(handler->Attributes.Normal);

}


void RenderComponent::SetShaderHandle(int id) {

	SetShaderHandle(ResMgr::getInstance()->getShaderProgramHandle(id));

}


void RenderComponent::SetShaderHandle(GLProgramHandle* handle) {

	RenderMgr::getInstance()->Remove(this);
	handler = handle;
	RenderMgr::getInstance()->Register(this);

}


void RenderComponent::SetVectorInfomation() {

	if (gameObject == nullptr) return;

	m_position = &Transform->m_position;
	m_scale = &Transform->m_scale;
	m_rotation = &Transform->m_rotation;

}
