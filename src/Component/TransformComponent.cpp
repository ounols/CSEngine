#include "TransformComponent.h"


COMPONENT_CONSTRUCTOR(TransformComponent) {
	m_position = vec3{ 0, 0, 0 };
	m_rotation = Quaternion();
	m_scale = vec3{ 1, 1, 1 };
}


TransformComponent::~TransformComponent() {}


void TransformComponent::Init() {
}


void TransformComponent::Tick(float elapsedTime) {
}


void TransformComponent::Exterminate() {
}

mat4 TransformComponent::GetMatrix() const {
	mat4 scale = mat4::Scale(m_scale.x, m_scale.y, m_scale.z);
	mat4 translation = mat4::Translate(m_position.x, m_position.y, m_position.z);
	//юс╫ц rotation
	// mat4 rotationY = mat4::RotateY(m_rotation.y);
	mat3 rotation_qua = m_rotation.ToMatrix();
	mat4 rotation = mat4(rotation_qua);
	// vec3 rotation_vec = m_rotation.ToEulerAngle();
	// mat4 rotation = mat4::RotateX(rotation_vec.x) * mat4::RotateY(rotation_vec.y) * mat4::RotateZ(rotation_vec.z);
	
	return scale * rotation * translation;
}