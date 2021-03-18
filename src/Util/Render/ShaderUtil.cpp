//
// Created by ounols on 19. 4. 30.
//

#include "ShaderUtil.h"
#include "../MoreString.h"

#ifdef WIN32
#include <Windows.h>
#endif

using namespace CSE;

ShaderUtil::ShaderUtil() {

}

ShaderUtil::~ShaderUtil() {

}


GLProgramHandle*
ShaderUtil::CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource, GLProgramHandle* handle) {
    if (vertexSource == nullptr || fragmentSource == nullptr) return nullptr;
    if (handle != nullptr && handle->Program != HANDLE_NULL) return nullptr;

    GLProgramHandle* gProgramhandle = handle;

    auto program = ShaderUtil::createProgram(vertexSource, fragmentSource);
    if (!program) {
        return nullptr;
    }

    //Find important variables from shader.
    auto variables_vert = GetImportantVariables(vertexSource);
    auto variables_frag = GetImportantVariables(fragmentSource);

    if (gProgramhandle == nullptr)
        gProgramhandle = new GLProgramHandle();
    gProgramhandle->SetProgram(program);
    //Get all variables from shader.
    gProgramhandle->SetAttributesList(variables_vert, variables_frag);
    gProgramhandle->SetUniformsList(variables_vert, variables_frag);

    //Binding important variables to engine.
    BindVariables(gProgramhandle);

    return gProgramhandle;
}


GLuint ShaderUtil::createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        return 0;
    }

    return createProgram(vertexShader, fragmentShader);
}


GLuint ShaderUtil::createProgram(GLuint vertexShader, GLuint fragmentShader) {
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

                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    std::cout << "Could not link program:\n" << buf << '\n';
                    //LOGE("Could not link program:\n%s\n", buf);
                    free(buf);

                }
            }

            glDeleteProgram(program);
            program = 0;

        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);

    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint ShaderUtil::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);

    if (shader) {

        glShaderSource(shader, 1, &pSource, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {

            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen) {

                auto buf = static_cast<char*>(malloc(infoLen));

                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    //LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    std::cout << "Could not compile shader:\n" << buf << '\n';
#ifdef WIN32
                    OutputDebugStringA(buf);
#endif
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
    std::map<std::string, std::string> variables;

    std::vector<std::string> str_line = split(source, ';');

    std::string type_str = "";

    for (auto line : str_line) {
        line += ';';
        auto result = split(line, '\n');

        for (auto str : result) {
            int start_index = 0;
            int end_index = 0;
            if ((start_index = str.find("//[")) != std::string::npos) {
                start_index += 3;
                end_index = str.find("]//");
                type_str = str.substr(start_index, end_index - 3);
                continue;
            }

            int eoc_index = 0;
            if (type_str != "" && (eoc_index = str.find(';')) != std::string::npos) {
                int start_index = str.substr(0, eoc_index).rfind(' ');
                int end_index = str.rfind('[');
                end_index = end_index == std::string::npos ? eoc_index : end_index;
                auto detail = str.substr(start_index, end_index - start_index);
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
    auto position = handle->AttributeLocation("POSITION");
    auto normal = handle->AttributeLocation("NORMAL");
    auto jointId = handle->AttributeLocation("JOINT_INDICES");
    auto weight = handle->AttributeLocation("WEIGHTS");
    auto textureCoord = handle->AttributeLocation("TEX_UV");
    auto color = handle->AttributeLocation("COLOR");
    //Uniforms
    auto modelView = handle->UniformLocation("MODELVIEW_MATRIX");
    auto modelViewNoCamera = handle->UniformLocation("MODELVIEW_NOCAMERA_MATRIX");
    auto cameraPosition = handle->UniformLocation("CAMERA_POSITION");
    auto projection = handle->UniformLocation("PROJECTION_MATRIX");
    auto skinningMode = handle->UniformLocation("SKINNING_MODE");
    auto jointMatrix = handle->UniformLocation("JOINT_MATRIX");

    handle->Attributes.Position = position != nullptr ? position->id : HANDLE_NULL;
    handle->Attributes.Normal = normal != nullptr ? normal->id : HANDLE_NULL;
    handle->Attributes.JointId = jointId != nullptr ? jointId->id : HANDLE_NULL;
    handle->Attributes.Weight = weight != nullptr ? weight->id : HANDLE_NULL;
    handle->Attributes.TextureCoord = textureCoord != nullptr ? textureCoord->id : HANDLE_NULL;
    handle->Attributes.Color = color != nullptr ? color->id : HANDLE_NULL;

    handle->Uniforms.Modelview = modelView != nullptr ? modelView->id : HANDLE_NULL;
    handle->Uniforms.ModelNoCameraMatrix = modelViewNoCamera != nullptr ? modelViewNoCamera->id : HANDLE_NULL;
    handle->Uniforms.CameraPosition = cameraPosition != nullptr ? cameraPosition->id : HANDLE_NULL;
    handle->Uniforms.Projection = projection != nullptr ? projection->id : HANDLE_NULL;
    handle->Uniforms.SkinningMode = skinningMode != nullptr ? skinningMode->id : HANDLE_NULL;
    handle->Uniforms.JointMatrix = jointMatrix != nullptr ? jointMatrix->id : HANDLE_NULL;
}
