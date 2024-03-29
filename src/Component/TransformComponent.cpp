#include "TransformComponent.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(TransformComponent) {
    m_position = vec3{ 0, 0, 0 };
    m_rotation = Quaternion();
    m_scale = vec3{ 1, 1, 1 };
}


TransformComponent::~TransformComponent() = default;


void TransformComponent::Init() {
}


void TransformComponent::Tick(float elapsedTime) {

    m_f_matrix = GetFinalMatrix();
    m_f_position = vec3{ m_f_matrix.w.x, m_f_matrix.w.y, m_f_matrix.w.z };
    m_f_scale = vec3{ m_f_matrix.x.x, m_f_matrix.y.y, m_f_matrix.z.z };

}


void TransformComponent::Exterminate() {
}

mat4 TransformComponent::GetMatrix() const {
    return m_f_matrix;
}

vec3* TransformComponent::GetPosition() {
    return &m_f_position;
}

vec3 TransformComponent::GetScale() const {
    return m_f_scale;
}

Quaternion TransformComponent::GetRotation() const {

    Quaternion parent = Quaternion();
    if (gameObject->GetParent() != nullptr) {
        parent = static_cast<TransformComponent*>(gameObject->GetParent()->GetTransform())->GetRotation();
    }


    return m_rotation.Multiplied(parent);
}

void TransformComponent::SetMatrix(const mat4& matrix) {
    m_position = vec3{ matrix.w.x, matrix.w.y, matrix.w.z };
    m_scale = vec3{ matrix.x.x, matrix.y.y, matrix.z.z };
    m_rotation = Quaternion::ToQuaternion(matrix);

}

mat4 TransformComponent::GetFinalMatrix() const {
    mat4 scale = mat4::Scale(m_scale.x, m_scale.y, m_scale.z);
    mat4 translation = mat4::Translate(m_position.x, m_position.y, m_position.z);
    //юс╫ц rotation
    // mat4 rotationY = mat4::RotateY(m_rotation.y);
    mat4 rotation = mat4(m_rotation.ToMatrix3());
    // vec3 rotation_vec = m_rotation.ToEulerAngle();
    // mat4 rotation = mat4::RotateX(rotation_vec.x) * mat4::RotateY(rotation_vec.y) * mat4::RotateZ(rotation_vec.z);

    mat4 parentMatrix = mat4::Identity();
    if (gameObject->GetParent() != nullptr) {
        parentMatrix = static_cast<TransformComponent*>(gameObject->GetParent()->GetTransform())->GetMatrix();
    }


    return scale * rotation * translation * parentMatrix;
}

SComponent* TransformComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(TransformComponent, clone);

    clone->m_position = vec3(m_position);
    clone->m_scale = vec3(m_scale);
    clone->m_rotation = Quaternion(m_rotation);

    return clone;
}

void TransformComponent::SetValue(std::string name_str, Arguments value) {
    if (name_str == "m_position") {
        m_position = vec3(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]));
    } else if (name_str == "m_scale") {
        m_scale = vec3(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]));
    } else if (name_str == "m_rotation") {
        m_rotation = Quaternion(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));
    }
}

std::string TransformComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE(m_position, m_position.x, ' ', m_position.y, ' ', m_position.z);
    PRINT_VALUE(m_scale, m_scale.x, ' ', m_scale.y, ' ', m_scale.z);
    PRINT_VALUE(m_rotation, m_rotation.x, ' ', m_rotation.y, ' ', m_rotation.z, ' ', m_rotation.w);

    PRINT_END("component");
}
