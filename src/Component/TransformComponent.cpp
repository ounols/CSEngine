#include "TransformComponent.h"


TransformComponent::TransformComponent() {
	m_position = vec3{ 0, 0, 0 };
	m_rotation = vec3{ 0, 0, 0 };
	m_scale = vec3{ 1, 1, 1 };
}


TransformComponent::~TransformComponent() {}


void TransformComponent::Tick(float elapsedTime) {
}


void TransformComponent::Exterminate() {
}
