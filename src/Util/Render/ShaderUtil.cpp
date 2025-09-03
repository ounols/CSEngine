//
// Created by ounols on 19. 4. 30.
//

#include "ShaderUtil.h"
#include "../MoreString.h"
#include "SEnvironmentMgr.h"
#include "../Settings.h"
#include "../../PlatformDef.h"

using namespace CSE;

#if defined(__CSE_DESKTOP__)
const char *ShaderUtil::m_defineVersion = "#version 330 core\n";
#elif defined(__CSE_ES__)
const char* ShaderUtil::m_defineVersion = "#version 300 es\n";
#endif

ShaderUtil::ShaderUtil() = default;

ShaderUtil::~ShaderUtil() = default;

GLProgramHandle*
ShaderUtil::CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource, GLProgramHandle* handle) {
    if (vertexSource == nullptr || fragmentSource == nullptr) return nullptr;
    if (handle != nullptr && handle->Program != HANDLE_NULL) return nullptr;

    GLProgramHandle* newHandle = handle;
    auto program = createProgram(vertexSource, fragmentSource, *newHandle);
    if (!program) {
        return nullptr;
    }

    //Find important variables from shader.
    auto variables_vert = GetImportantVariables(vertexSource);
    auto variables_frag = GetImportantVariables(fragmentSource);

    if (newHandle == nullptr)
        newHandle = new GLProgramHandle();
    newHandle->SetProgram(program);
    //Get all variables from shader.
    newHandle->SetAttributesList(variables_vert, variables_frag);
    newHandle->SetUniformsList(variables_vert, variables_frag);

    //Binding important variables to engine.
    BindVariables(newHandle);

    return newHandle;
}

GLuint
ShaderUtil::createProgram(const GLchar* vertexSource, const GLchar* fragmentSource, const GLProgramHandle& handle) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource, handle);
    if (!vertexShader) {
        return 0;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource, handle);
    if (!fragmentShader) {
        return 0;
    }

    return createProgram(vertexShader, fragmentShader, handle);
}

GLuint ShaderUtil::createProgram(GLuint vertexShader, GLuint fragmentShader, const GLProgramHandle& handle) {
    GLuint program = glCreateProgram();

    if (program) {
        //쉐이더를 attach 합니다.
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

            if (bufLength) {
                auto buf = static_cast<char*>(malloc(bufLength));

                if (buf) {
                    glGetProgramInfoLog(program, bufLength, nullptr, buf);
                    auto errorLog = std::string("[") + handle.GetName() + "] Could not link program:" + buf;
                    SafeLog::Log(errorLog.c_str());
                    free(buf);

                }
            }
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
            return 0;
        }
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);

    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint ShaderUtil::loadShader(GLenum shaderType, const char* pSource, const GLProgramHandle& handle) {
    GLuint shader = glCreateShader(shaderType);
    std::string srcString = std::string(m_defineVersion)
                            + "#define MAX_JOINTS " + std::to_string(Settings::GetMaxJoints()) + '\n'
                            + "#define MAX_LIGHTS " + std::to_string(Settings::GetMaxLights()) + '\n'
                            + pSource;
    const char* src = srcString.c_str();

    if (shader) {
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen) {
                auto buf = static_cast<char*>(malloc(infoLen));

                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, nullptr, buf);
                    //LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    auto errorLog = std::string("[") + handle.GetName() + "] Could not compile shader:" + buf;
                    SafeLog::Log(errorLog.c_str());
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

std::map<std::string, std::string> ShaderUtil::GetImportantVariables(const GLchar* source) {
    // source를 각 라인별로 나누기 위해 std::stringstream을 사용
    std::stringstream ss(source);
    std::string line;
    std::map<std::string, std::string> variables;
    std::string type_str;

    // std::stringstream에서 각 라인을 읽음
    while (std::getline(ss, line)) {
        // 변수 타입을 포함하는 주석 블록의 시작과 끝 인덱스를 찾음
        int start_index = line.find("//[");
        int end_index = line.find("]//");

        if (start_index != std::string::npos && end_index != std::string::npos) {
            // 시작 인덱스를 3 증가시킴 (주석 블록을 제외한 타입 이름의 시작 인덱스)
            start_index += 3;
            // 타입 이름을 추출
            type_str = line.substr(start_index, end_index - 3);
            continue;
        }

        // 타입 이름이 찾아졌으면, 변수 선언을 이 라인에서 찾음
        if (!type_str.empty()) {
            // 문장의 끝을 나타내는 세미콜론의 인덱스를 찾음
            int eoc_index = line.find(';');

            if (eoc_index != std::string::npos) {
                // 변수 이름의 시작과 끝 인덱스를 찾음
                int startIndex = line.substr(0, eoc_index).rfind(' ');
                int endIndex = line.rfind('[');
                endIndex = endIndex == std::string::npos ? eoc_index : endIndex;
                auto detail = line.substr(startIndex, endIndex - startIndex);

                // 변수 이름의 공백을 제거하고 맵에 추가
                detail = trim(detail);
                variables[type_str] = detail;
                type_str.clear();
            }
        }
    }

    return variables;
}

void ShaderUtil::BindVariables(GLProgramHandle* handle) {
    if (handle == nullptr) return;

    //Attributes
    auto position = handle->AttributeLocation("att.position");
    auto normal = handle->AttributeLocation("att.normal");
    auto jointId = handle->AttributeLocation("att.joint_indices");
    auto weight = handle->AttributeLocation("att.weight");
    auto textureCoord = handle->AttributeLocation("att.tex_uv");
    auto color = handle->AttributeLocation("att.color");
    //Uniforms
    auto view = handle->UniformLocation("matrix.view");
    auto model = handle->UniformLocation("matrix.model");
    auto cameraPosition = handle->UniformLocation("vec3.camera");
    auto projection = handle->UniformLocation("matrix.projection");
    auto projectionInv = handle->UniformLocation("matrix.projection.inv");
    auto viewInv = handle->UniformLocation("matrix.view.inv");
    auto skinningMode = handle->UniformLocation("matrix.skinning_mode");
    auto jointMatrix = handle->UniformLocation("matrix.joint");
    auto lightPosition = handle->UniformLocation("light.position");
    auto lightType = handle->UniformLocation("light.type");
    auto lightRadius = handle->UniformLocation("light.radius");
    auto lightColor = handle->UniformLocation("light.color");
    auto lightShadowMap = handle->UniformLocation("light.shadow_map");
    auto lightMatrix = handle->UniformLocation("light.matrix");
    auto lightShadowMode = handle->UniformLocation("light.shadow_mode");
    auto lightSize = handle->UniformLocation("light.size");
    auto lightIrradiance = handle->UniformLocation("light.irradiance");
    auto lightPrefilter = handle->UniformLocation("light.prefilter");
    auto lightBrdf = handle->UniformLocation("light.brdf");
    auto srcBuffer = handle->UniformLocation("buffer.source");
    auto srcBufferSize = handle->UniformLocation("buffer.source.size");
    auto sdfEnvSize = handle->UniformLocation("sdf.env.size");
    auto sdfCellSize = handle->UniformLocation("sdf.cell.size");
    auto sdfNodeSize = handle->UniformLocation("sdf.node.size");
    auto sdfNodeSpace = handle->UniformLocation("sdf.node.space");
    auto sdfFrameCount = handle->UniformLocation("sdf.frame.count");
    auto sdfMap = handle->UniformLocation("texture.sdfmap");

    handle->Attributes.Position = position != nullptr ? position->id : HANDLE_NULL;
    handle->Attributes.Normal = normal != nullptr ? normal->id : HANDLE_NULL;
    handle->Attributes.JointId = jointId != nullptr ? jointId->id : HANDLE_NULL;
    handle->Attributes.Weight = weight != nullptr ? weight->id : HANDLE_NULL;
    handle->Attributes.TextureCoord = textureCoord != nullptr ? textureCoord->id : HANDLE_NULL;
    handle->Attributes.Color = color != nullptr ? color->id : HANDLE_NULL;

    handle->Uniforms.ViewMatrix = view != nullptr ? view->id : HANDLE_NULL;
    handle->Uniforms.ModelMatrix = model != nullptr ? model->id : HANDLE_NULL;
    handle->Uniforms.CameraPosition = cameraPosition != nullptr ? cameraPosition->id : HANDLE_NULL;
    handle->Uniforms.ProjectionMatrix = projection != nullptr ? projection->id : HANDLE_NULL;
    handle->Uniforms.ProjectionInvMatrix = projectionInv != nullptr ? projectionInv->id : HANDLE_NULL;
    handle->Uniforms.ViewInvMatrix = viewInv != nullptr ? viewInv->id : HANDLE_NULL;
    handle->Uniforms.SkinningMode = skinningMode != nullptr ? skinningMode->id : HANDLE_NULL;
    handle->Uniforms.JointMatrix = jointMatrix != nullptr ? jointMatrix->id : HANDLE_NULL;
    handle->Uniforms.LightPosition = lightPosition != nullptr ? lightPosition->id : HANDLE_NULL;
    handle->Uniforms.LightType = lightType != nullptr ? lightType->id : HANDLE_NULL;
    handle->Uniforms.LightRadius = lightRadius != nullptr ? lightRadius->id : HANDLE_NULL;
    handle->Uniforms.LightColor = lightColor != nullptr ? lightColor->id : HANDLE_NULL;
    handle->Uniforms.LightShadowMap = lightShadowMap != nullptr ? lightShadowMap->id : HANDLE_NULL;
    handle->Uniforms.LightMatrix = lightMatrix != nullptr ? lightMatrix->id : HANDLE_NULL;
    handle->Uniforms.LightShadowMode = lightShadowMode != nullptr ? lightShadowMode->id : HANDLE_NULL;
    handle->Uniforms.LightSize = lightSize != nullptr ? lightSize->id : HANDLE_NULL;
    handle->Uniforms.LightIrradiance = lightIrradiance != nullptr ? lightIrradiance->id : HANDLE_NULL;
    handle->Uniforms.LightPrefilter = lightPrefilter != nullptr ? lightPrefilter->id : HANDLE_NULL;
    handle->Uniforms.LightBrdfLut = lightBrdf != nullptr ? lightBrdf->id : HANDLE_NULL;
    handle->Uniforms.SourceBuffer = srcBuffer != nullptr ? srcBuffer->id : HANDLE_NULL;
    handle->Uniforms.SourceBufferSize = srcBufferSize != nullptr ? srcBufferSize->id : HANDLE_NULL;
    handle->Uniforms.SdfEnvSize = sdfEnvSize != nullptr ? sdfEnvSize->id : HANDLE_NULL;
    handle->Uniforms.SdfCellSize = sdfCellSize != nullptr ? sdfCellSize->id : HANDLE_NULL;
    handle->Uniforms.SdfNodeSize = sdfNodeSize != nullptr ? sdfNodeSize->id : HANDLE_NULL;
    handle->Uniforms.SdfNodeSpace = sdfNodeSpace != nullptr ? sdfNodeSpace->id : HANDLE_NULL;
    handle->Uniforms.SdfFrameCount = sdfFrameCount != nullptr ? sdfFrameCount->id : HANDLE_NULL;
    handle->Uniforms.SdfMap = sdfMap != nullptr ? sdfMap->id : HANDLE_NULL;
}

void ShaderUtil::BindCameraToShader(const GLProgramHandle& handle, const mat4& view, const vec3& cameraPosition,
                                    const mat4& projection, const mat4& transform) {
    if(handle.Uniforms.ViewMatrix >= 0)
        glUniformMatrix4fv(handle.Uniforms.ViewMatrix, 1, 0, view.Pointer());
    if(handle.Uniforms.ModelMatrix >= 0)
        glUniformMatrix4fv(handle.Uniforms.ModelMatrix, 1, 0, transform.Pointer());
    if(handle.Uniforms.ProjectionMatrix >= 0)
        glUniformMatrix4fv(handle.Uniforms.ProjectionMatrix, 1, 0, projection.Pointer());
    if(handle.Uniforms.CameraPosition >= 0)
        glUniform3fv(handle.Uniforms.CameraPosition, 1, cameraPosition.Pointer());

    if(handle.Uniforms.ProjectionInvMatrix >= 0)
        glUniformMatrix4fv(handle.Uniforms.ProjectionInvMatrix, 1, 0, mat4::Invert(projection).Pointer());
    if(handle.Uniforms.ViewInvMatrix >= 0)
        glUniformMatrix4fv(handle.Uniforms.ViewInvMatrix, 1, 0, mat4::Invert(view).Pointer());
}

// position(x y z) + normal(x y z) + tex(u v) + weight(x y z) + jointID(id1, id2, id3)
void ShaderUtil::BindAttributeToShader(const GLProgramHandle& handle, const GLMeshID& meshId) {
    int stride = 4 * sizeof(vec3) + sizeof(vec2); //normal + position + uv = (4 * sizeof(vec3) + sizeof(vec2))
    // 필요한 속성 정의
    struct Attribute {
        int id; // 속성 ID
        size_t offset; // 오프셋
        int size; // 속성의 개수
    };

    // 필요한 속성들을 정의
    const std::vector<Attribute> attributes = {
            { handle.Attributes.Position,     0,                               3 },
            { handle.Attributes.Normal,       sizeof(vec3),                    3 },
            { handle.Attributes.TextureCoord, 2 * sizeof(vec3),                2 },
            { handle.Attributes.Weight,       2 * sizeof(vec3) + sizeof(vec2), 3 },
            { handle.Attributes.JointId,      3 * sizeof(vec3) + sizeof(vec2), 3 },
    };

    // 속성들을 바인딩
    glBindVertexArray(meshId.m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, meshId.m_vertexBuffer);
    for (const auto& attribute: attributes) {
        if (attribute.id < 0) continue; // 속성이 없을 경우 다음 속성으로
        glEnableVertexAttribArray(attribute.id);
        glVertexAttribPointer(attribute.id, attribute.size, GL_FLOAT, GL_FALSE, stride, (GLvoid*) attribute.offset);
    }

    // 인덱스가 있을 경우 인덱스 버퍼를 바인딩
    if (meshId.m_indexSize < 0) {
        glDrawArrays(GL_TRIANGLES, 0, meshId.m_vertexSize);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshId.m_indexBuffer);
        glDrawElements(GL_TRIANGLES, meshId.m_indexSize * 3, GL_UNSIGNED_SHORT, nullptr);
    }
    // 속성들의 바인딩을 해제
    for (const auto& attribute: attributes) {
        if (attribute.id < 0) continue; // 속성이 없을 경우 다음 속성으로
        glDisableVertexAttribArray(attribute.id);
    }
    glBindVertexArray(0);
}

void ShaderUtil::BindAttributeToPlane() {
    SEnvironmentMgr::RenderPlaneVAO();
}

void ShaderUtil::BindAttributeToCubeMap() {
    SEnvironmentMgr::RenderCubeVAO();
}

void ShaderUtil::BindSkinningDataToShader(const GLProgramHandle& handle, const GLMeshID& meshId,
                                          const std::vector<mat4>& jointMatrix) {
    if (!meshId.m_hasJoint || jointMatrix.empty()) {
        glUniform1i(handle.Uniforms.SkinningMode, 0);
        return;
    }

    std::vector<float> result;
    result.reserve(jointMatrix.size() * 16);
    for (const mat4& matrix: jointMatrix) {
        std::copy(matrix.Pointer(), matrix.Pointer() + 16, std::back_inserter(result));
    }

    glUniformMatrix4fv(handle.Uniforms.JointMatrix, Settings::GetMaxJoints(), GL_FALSE, &result[0]);
    glUniform1i(handle.Uniforms.SkinningMode, 1);
}