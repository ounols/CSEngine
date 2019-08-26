#include "../Util/GLProgramHandle.h"
#include "DrawableSkinnedMeshComponent.h"
#include "../OGLDef.h"

#define MeshComponent DrawableSkinnedMeshComponent

DrawableSkinnedMeshComponent::DrawableSkinnedMeshComponent() {
    m_classType = "DrawableSkinnedMeshComponent";
}

MeshComponent::~MeshComponent() {

}

void MeshComponent::SetRootJoint(SGameObject* joint_object, int joint_size) {
    m_jointRoot = joint_object->GetComponent<JointComponent>();
    m_jointSize = joint_size;

    m_jointRoot->calcInverseBindTransform(mat4::Identity());
    // SetJointSize(joint_object);
}

JointComponent* DrawableSkinnedMeshComponent::GetRootJoint() {
    return m_jointRoot;
}



std::vector<mat4> DrawableSkinnedMeshComponent::GetJointMatrix() const {
    std::vector<mat4> jointMatrices;

    if(m_jointRoot == nullptr) return jointMatrices;

    jointMatrices.resize(m_jointSize);
    addJointsToVector(m_jointRoot, jointMatrices);
    return jointMatrices;
}



void DrawableSkinnedMeshComponent::addJointsToVector(JointComponent* headJoint, std::vector<mat4>& matrix) const {
    int index = headJoint->GetID();
    matrix[index] = headJoint->GetAnimationMatrix();

    std::vector<SGameObject*> children = headJoint->GetGameObject()->GetChildren();
    for (SGameObject* childJoint : children) {
        auto joint = childJoint->GetComponent<JointComponent>();

        if(joint == nullptr) continue;
        addJointsToVector(joint, matrix);
    }
}

void DrawableSkinnedMeshComponent::SetJointSize(SGameObject* joint_object) {
    if(joint_object->GetComponent<JointComponent>() == nullptr) return;
    m_jointSize++;
    auto children = joint_object->GetChildren();
    for (SGameObject* childJoint : children) {
        SetJointSize(childJoint);
    }
}

bool DrawableSkinnedMeshComponent::SetMesh(const SISurface& meshSurface) {
    if(!DrawableStaticMeshComponent::SetMesh(meshSurface)) return false;

    CreateSkinningBuffers(meshSurface);

    return true;
}

bool DrawableSkinnedMeshComponent::AttachJointMatrix(const GLProgramHandle* handle) const {
    auto joints = GetJointMatrix();

    if(joints.empty()) return false;

    std::vector<float> result;

    for(mat4 matrix : joints) {
        for(int i = 0; i < 16; i++) {
            result.push_back(matrix.Pointer()[i]);

        }
    }

    glUniformMatrix4fv(handle->Uniforms.JointMatrix, MAX_JOINTS, 0, &result[0]);

    return true;
}

void DrawableSkinnedMeshComponent::CreateSkinningBuffers(const SISurface& surface) {

//    const MeshSurface& skinning_surface = (const MeshSurface&) surface;
//
//    std::vector<float> weights = skinning_surface.GetWeights();
//    GLuint weightsBuffer;
//    glGenBuffers(1, &weightsBuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, weightsBuffer);
//    glBufferData(GL_ARRAY_BUFFER,
//                 weights.size() * sizeof(weights[0]),
//                 &weights[0],
//                 GL_STATIC_DRAW);
//
//    auto jointIds_intager = skinning_surface.GetJointIDs();
//    std::vector<float> jointIds(jointIds_intager.begin(), jointIds_intager.end());
//    GLuint jointIndices;
//    glGenBuffers(1, &jointIndices);
//    glBindBuffer(GL_ARRAY_BUFFER, jointIndices);
//    glBufferData(GL_ARRAY_BUFFER,
//                 jointIds.size() * sizeof(jointIds[0]),
//                 &jointIds[0],
//                 GL_STATIC_DRAW);
//
//    //Pulling data
//    surface.m_meshId.m_weight = weightsBuffer;
//    surface.m_meshId.m_jointId = jointIndices;
//
//    //Unbinding
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

SComponent* DrawableSkinnedMeshComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(DrawableSkinnedMeshComponent, clone);

    clone->m_meshId = m_meshId;
    clone->m_jointSize = m_jointSize;

    return clone;
}

void DrawableSkinnedMeshComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                                 std::map<SComponent*, SComponent*> lists_comp) {
    if (src == nullptr) return;
    DrawableSkinnedMeshComponent* convert = dynamic_cast<DrawableSkinnedMeshComponent*>(src);

    //Copy Components
    FIND_COMP_REFERENCE(m_jointRoot, convert, JointComponent);
}
