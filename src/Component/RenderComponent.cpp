#include "RenderComponent.h"
#include "TransformComponent.h"
#include "DrawableSkinnedMeshComponent.h"
#include "../Manager/RenderMgr.h"

using namespace CSE;

COMPONENT_CONSTRUCTOR(RenderComponent) {
    RenderMgr::getInstance()->Register(this);
}


RenderComponent::~RenderComponent() {}


void RenderComponent::Exterminate() {
    RenderMgr::getInstance()->Remove(this);
}


void RenderComponent::Init() {

    SetVectorInfomation();
    m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();

    if (m_mesh != nullptr) {
        if (dynamic_cast<DrawableSkinnedMeshComponent*>(m_mesh)) {
            m_isSkinned = true;
        }
    }

    m_material = gameObject->GetComponent<MaterialComponent>();


    isRenderActive = isEnable;
}


void RenderComponent::Tick(float elapsedTime) {

    if (m_mesh == nullptr) {
        m_mesh = gameObject->GetComponent<DrawableStaticMeshComponent>();
        if (m_mesh != nullptr) {
            if (dynamic_cast<DrawableSkinnedMeshComponent*>(m_mesh)) {
                m_isSkinned = true;
            }
        }
    }

    if (m_material == nullptr) {
        m_material = gameObject->GetComponent<MaterialComponent>();
    }

}


void RenderComponent::SetMatrix(mat4 camera, vec3 cameraPosition, mat4 projection) {

    // //model-view
    // mat4 scale = mat4::Scale(m_scale->x, m_scale->y, m_scale->z);
    // mat4 translation = mat4::Translate(m_position->x, m_position->y, m_position->z);
    // //�ӽ� rotation
    // mat4 rotationY = mat4::RotateY(m_rotation->y);
    // //mat4 rotation =
    // mat4 modelNoCameraView = scale * /*rotation*/ rotationY * translation;
    mat4 modelNoCameraView = static_cast<TransformComponent*>(gameObject->GetTransform())->GetMatrix();
    mat4 modelView = modelNoCameraView * camera;
    glUniformMatrix4fv(handler->Uniforms.Modelview, 1, 0, modelView.Pointer());
    glUniformMatrix4fv(handler->Uniforms.ModelNoCameraMatrix, 1, 0, modelNoCameraView.Pointer());
    glUniform3fv(handler->Uniforms.CameraPosition, 1, cameraPosition.Pointer());

    //normal matrix
//    glUniformMatrix3fv(handler->Uniforms.NormalMatrix, 1, 0, modelView.ToMat3().Pointer());


    //projection transform
    glUniformMatrix4fv(handler->Uniforms.Projection, 1, 0, projection.Pointer());


}


void RenderComponent::Render(float elapsedTime) {

    if (m_mesh == nullptr) return;

    SetMaterials();
    SetJointMatrix();


    int stride = 4 * sizeof(vec3) + sizeof(vec2);    //normal + position + uv
    const auto& drawable_id = m_mesh->m_meshId;
    const GLvoid* offset = (const GLvoid*) sizeof(vec3);
    GLint position = handler->Attributes.Position;
    GLint normal = handler->Attributes.Normal;
    GLint tex = handler->Attributes.TextureCoord;
    GLint weight = handler->Attributes.Weight;
    GLint jointId = handler->Attributes.JointId;

    bool isTex = tex != HANDLE_NULL;

    if (drawable_id.m_indexSize < 0) {

        glBindBuffer(GL_ARRAY_BUFFER, drawable_id.m_vertexBuffer);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
        if (isTex) {
            offset = (GLvoid*) (sizeof(vec3) * 2);
            glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, stride, offset);
        }
        if (m_isSkinned) {
            offset = (GLvoid*) (sizeof(vec3) * 2 + sizeof(vec2));
            glVertexAttribPointer(weight, 3, GL_FLOAT, GL_FALSE, stride, offset);

            offset = (GLvoid*) (sizeof(vec3) * 3 + sizeof(vec2));
            glVertexAttribPointer(jointId, 3, GL_FLOAT, GL_FALSE, stride, offset);
        }

        glDrawArrays(GL_TRIANGLES, 0, drawable_id.m_vertexSize);

    } else {
        glBindBuffer(GL_ARRAY_BUFFER, drawable_id.m_vertexBuffer);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
        if (isTex) {
            offset = (GLvoid*) (sizeof(vec3) * 2);
            glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, stride, offset);
        }
        if (m_isSkinned) {
            offset = (GLvoid*) (sizeof(vec3) * 2 + sizeof(vec2));
            glVertexAttribPointer(weight, 3, GL_FLOAT, GL_FALSE, stride, offset);

            offset = (GLvoid*) (sizeof(vec3) * 3 + sizeof(vec2));
            glVertexAttribPointer(jointId, 3, GL_FLOAT, GL_FALSE, stride, offset);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable_id.m_indexBuffer);


        glDrawElements(GL_TRIANGLES, drawable_id.m_indexSize * 3, GL_UNSIGNED_SHORT, 0);

    }

}


void RenderComponent::SetShaderHandle(std::string name) {
    SetShaderHandle(ResMgr::getInstance()->GetObject<GLProgramHandle>(name));

}


void RenderComponent::SetShaderHandle(GLProgramHandle* handle) {

    RenderMgr::getInstance()->Remove(this);
    handler = handle;
    RenderMgr::getInstance()->Register(this);

}


void RenderComponent::SetIsEnable(bool is_enable) {
    SComponent::SetIsEnable(is_enable);

    isRenderActive = isEnable;
}


void RenderComponent::SetVectorInfomation() {

    if (gameObject == nullptr) return;

}


void RenderComponent::SetMaterials() const {

    //Set Materials

    if (m_material == nullptr) {
        //Default Error Material
        vec3 m_ambientMaterial = vec3{ 1.f, 0.f, 0.f };
        vec3 m_specularMaterial = vec3{ 1, 1, 1 };
        float m_shininess = 128;

//        glUniform3fv(handler->Uniforms.AmbientMaterial, 1, m_ambientMaterial.Pointer());
//        glUniform3fv(handler->Uniforms.SpecularMaterial, 1, m_specularMaterial.Pointer());
//        glUniform1f(handler->Uniforms.Shininess, m_shininess);

    } else {
        m_material->AttachMaterials(handler);
    }


}

SComponent* RenderComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(RenderComponent, clone);

    clone->isRenderActive = isRenderActive;
    clone->SetShaderHandle(handler);

    return clone;
}

void RenderComponent::SetJointMatrix() {
    if (!m_isSkinned) {
        glUniform1i(handler->Uniforms.IsSkinning, 0);
        return;
    }

    auto mesh = dynamic_cast<DrawableSkinnedMeshComponent*>(m_mesh);

    if (mesh->AttachJointMatrix(handler)) {
        glUniform1i(handler->Uniforms.IsSkinning, 1);
    }
}
