#include "CameraComponent.h"
#include "TransformComponent.h"
#include "../Manager/CameraMgr.h"
#include "../Manager/EngineCore.h"
#include "../Util/Render/SFrameBuffer.h"
#include "../Util/Render/STexture.h"
#include "../Util/Render/ShaderUtil.h"

#include <mutex>

using namespace CSE;

COMPONENT_CONSTRUCTOR(CameraComponent), m_eye(nullptr), m_targetObject(nullptr) {
    auto cameraMgr = CORE->GetCore(CameraMgr);
    cameraMgr->Register(this);
    m_pRatio = const_cast<float*>(cameraMgr->GetProjectionRatio());
    m_globalSkyboxHandle = cameraMgr->GetSkyboxProgram();
}

CameraComponent::~CameraComponent() = default;

void CameraComponent::Exterminate() {
    SAFE_DELETE(m_backgroundMap);
    CORE->GetCore(CameraMgr)->Remove(this);
}

void CameraComponent::Init() {
    m_eye = static_cast<TransformComponent*>(gameObject->GetTransform())->GetPosition();

    m_resultTarget = vec3();
    m_pRatio = const_cast<float*>(CORE->GetCore(CameraMgr)->GetProjectionRatio());
    Tick(0);
}

void CameraComponent::Tick(float elapsedTime) {
    if(m_type == CUBE) return;

    if (m_targetObject == nullptr)
        m_resultTarget = *m_eye + m_target;
    else {
        m_resultTarget = *static_cast<TransformComponent*>(m_targetObject->GetTransform())->GetPosition();
    }
    SetCameraMatrix();
}

SComponent* CameraComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(CameraComponent, comp);

    comp->m_eye = m_eye;
    comp->m_target = m_target;
    comp->m_up = m_up;

    comp->m_cameraMatrix = m_cameraMatrix;
    comp->m_projectionMatrix = m_projectionMatrix;
    comp->m_resultTarget = m_resultTarget;
    comp->m_frameBuffer = m_frameBuffer;

    comp->m_type = m_type;
    comp->m_isProjectionInited = m_isProjectionInited;

    //perspective
    comp->m_pFov = m_pFov;
    comp->m_pRatio = m_pRatio;

    //Orthographic
    comp->m_oLeft = m_oLeft;
    comp->m_oRight = m_oRight;
    comp->m_oBottom = m_oBottom;
    comp->m_oTop = m_oTop;

    comp->m_Near = m_Near;
    comp->m_Far = m_Far;

    //Background
    comp->m_backgroundType = m_backgroundType;
    comp->m_backgroundColor = m_backgroundColor;
    if(comp->m_backgroundMap == nullptr) comp->m_backgroundMap = new BackgroundMapStruct();
    comp->m_backgroundMap->map = m_backgroundMap->map;
    comp->m_backgroundMap->mapId = m_backgroundMap->mapId;
    comp->m_backgroundMap->viewId = m_backgroundMap->viewId;
    comp->m_backgroundMap->projectionId = m_backgroundMap->projectionId;

    return comp;
}

void CameraComponent::CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                    std::map<SComponent*, SComponent*> lists_comp) {
    if (src == nullptr) return;
    auto convert = static_cast<CameraComponent*>(src);

    //Copy GameObjects
    FIND_OBJ_REFERENCE(m_targetObject, convert);

}

void CameraComponent::SetTargetVector(const vec3& target) {
    m_target = target;
}

void CameraComponent::SetTarget(SGameObject* gameObject) {
    m_targetObject = gameObject;
}

void CameraComponent::SetUp(const vec3& up) {
    m_up = up;
}

void CameraComponent::SetCameraType(CAMERATYPE type) {
    m_type = type;
}

void CameraComponent::SetPerspectiveFov(float fov) {
    m_pFov = fov;
}

void CameraComponent::SetZDepthRange(float near, float far) {
    m_Near = near;
    m_Far = far;

}

void CameraComponent::SetPerspective(float fov, float near, float far) {
    m_type = PERSPECTIVE;
    m_pFov = fov;
    m_Near = near;
    m_Far = far;

}

void CameraComponent::SetOrtho(float left, float right, float top, float bottom) {
    m_type = ORTHO;
    m_oLeft = left;
    m_oRight = right;
    m_oBottom = bottom;
    m_oTop = top;
}

void CameraComponent::SetCubeCamera() {
    m_type = CUBE;
}

void CameraComponent::SetCameraMatrix() {
    m_cameraMatrix = mat4::LookAt(*m_eye, m_resultTarget, m_up);
}

mat4 CameraComponent::GetCameraMatrix() const {
    return m_cameraMatrix;
}

vec3 CameraComponent::GetCameraPosition() const {
    mat4 matrix = static_cast<TransformComponent*>(gameObject->GetTransform())->GetMatrix();
    return vec3{ matrix.w.x, matrix.w.y, matrix.w.z };
}

void CameraComponent::SetProjectionMatrix() const {
	std::mutex mutex;
	mutex.lock();

    switch (m_type) {
        case PERSPECTIVE:
            m_projectionMatrix = mat4::Perspective(m_pFov, *m_pRatio, m_Near, m_Far);
            break;
        case ORTHO:
            m_projectionMatrix = mat4::Ortho(m_oLeft, m_oRight, m_oTop, m_oBottom, m_Near, m_Far);
            break;
        case CUBE:
            m_projectionMatrix = mat4::Perspective(90.0f, 1.0f, 0.1f, 10.0f);
            break;
    }

    m_isProjectionInited = true;
	mutex.unlock();
}

void CameraComponent::SetValue(std::string name_str, VariableBinder::Arguments value) {
    if (name_str == "m_target") {
        SET_VEC3(m_target);
    } else if (name_str == "m_up") {
        SET_VEC3(m_up);
    } else if (name_str == "m_targetObject") {
        if(value.size() <= 0) return;
        m_targetObject = SGameObject::FindByHash(value[0]);
    } else if (name_str == "m_cameraMatrix") {
        SET_MAT4(m_cameraMatrix);
    } else if (name_str == "m_projectionMatrix") {
        SET_MAT4(m_projectionMatrix);
    } else if (name_str == "m_type") {
        m_type = static_cast<CAMERATYPE>(std::stoi(value[0]));
    } else if (name_str == "m_pFov") {
        m_pFov = std::stof(value[0]);
    } else if (name_str == "m_orthoValue") {
        m_oLeft = std::stof(value[0]);
        m_oRight = std::stof(value[1]);
        m_oBottom = std::stof(value[2]);
        m_oTop = std::stof(value[3]);

    } else if (name_str == "m_distance") {
        m_Near = std::stof(value[0]);
        m_Far = std::stof(value[1]);
    } else if (name_str == "m_frameBuffer") {
        m_frameBuffer = SResource::Create<SFrameBuffer>(value[0]);
    } else if (name_str == "m_backgroundType") {
        m_backgroundType = static_cast<BackgroundType>(std::stoi(value[0]));
    } else if (name_str == "m_backgroundColor") {
        SET_VEC3(m_backgroundColor);
    } else if (name_str == "m_backgroundMap.map") {
        SetBackgroundSkybox(SResource::Create<STexture>(value[0]));
    }

}

std::string CameraComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE_VEC3(m_target);
    PRINT_VALUE_VEC3(m_up);
    PRINT_VALUE("obj", m_targetObject, m_targetObject == nullptr ? "" : ConvertSpaceStr(m_targetObject->GetHash()));
    PRINT_VALUE_MAT4(m_cameraMatrix);
    PRINT_VALUE_MAT4(m_projectionMatrix);
    PRINT_VALUE("int", m_type, static_cast<int>(m_type));
    PRINT_VALUE("float", m_pFov, m_pFov);
    PRINT_VALUE("vec4", m_orthoValue, m_oLeft, ' ', m_oRight, ' ', m_oBottom, ' ', m_oTop);
    PRINT_VALUE("vec2", m_distance, m_Near, ' ', m_Far);
    if (m_frameBuffer != nullptr) PRINT_RES_NAME(m_frameBuffer, m_frameBuffer);

    PRINT_VALUE("int", m_backgroundType, static_cast<int>(m_backgroundType));
    PRINT_VALUE_COLOR3(m_backgroundColor);
    if (m_backgroundMap != nullptr && m_backgroundMap->map != nullptr)
        PRINT_RES_NAME(m_backgroundMap.map, m_backgroundMap->map);

    PRINT_END("component");
}

CameraMatrixStruct CameraComponent::GetCameraMatrixStruct() const {
    return { m_cameraMatrix, GetProjectionMatrix(), GetCameraPosition() };
}

SFrameBuffer* CameraComponent::GetFrameBuffer() const {
    return m_frameBuffer;
}

void CameraComponent::SetFrameBuffer(SFrameBuffer* frameBuffer) {
    m_frameBuffer = frameBuffer;
}

CameraBase::BackgroundType CameraComponent::GetBackgroundType() {
    return m_backgroundType;
}

void CameraComponent::RenderBackground() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (m_backgroundType) {
        case SOLID:
            glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            break;
        case SKYBOX:
            const auto& mapStruct = m_backgroundMap;
            if (mapStruct->map == nullptr) return;

            glUseProgram(m_globalSkyboxHandle->Program);
            m_globalSkyboxHandle->SetUniformMat4("PROJECTION_MATRIX", m_projectionMatrix);
            auto viewMatrix = mat4(m_cameraMatrix.ToMat3());
            m_globalSkyboxHandle->SetUniformMat4("VIEW_MATRIX", viewMatrix);
            mapStruct->map->Bind(mapStruct->mapId, 0);

            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            ShaderUtil::BindAttributeToCubeMap();
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            break;
    }
}

void CameraComponent::SetBackgroundSkybox(STexture* skyboxTexture) {
    if (skyboxTexture == nullptr) {
        skyboxTexture = SResource::Get<STexture>("envCubemap.textureCubeMap");
    }
    if (m_backgroundMap == nullptr) m_backgroundMap = new BackgroundMapStruct();

    m_backgroundMap->map = skyboxTexture;
    m_backgroundMap->mapId = static_cast<unsigned short>(m_globalSkyboxHandle->UniformLocation("ENV_MAP")->id);
    m_backgroundMap->viewId = static_cast<unsigned short>(m_globalSkyboxHandle->UniformLocation("VIEW_MATRIX")->id);
    m_backgroundMap->projectionId = static_cast<unsigned short>(m_globalSkyboxHandle->UniformLocation(
            "PROJECTION_MATRIX")->id);
}

void CameraComponent::SetBackgroundColor(vec3&& color) {
    m_backgroundColor = color;
}

void CameraComponent::SetBackgroundType(CameraBase::BackgroundType type) {
    m_backgroundType = type;
}