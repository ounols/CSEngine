#pragma once

#include "../OGLDef.h"
#include "../SObject.h"
#include "../Manager/ResMgr.h"
#include "../Manager/ShaderProgramContainer.h"
#include "Matrix.h"

#include <map>

#define HANDLE_NULL -1
#define MAX_LIGHTS 16
#define MAX_JOINTS 60

struct GLUniformHandles {
    GLint Modelview = HANDLE_NULL;
    GLint ModelNoCameraMatrix = HANDLE_NULL;
    GLint Projection = HANDLE_NULL;
    GLint CameraPosition = HANDLE_NULL;
    GLint LightPosition = HANDLE_NULL;
    GLint LightType = HANDLE_NULL;
    GLint LightRadius = HANDLE_NULL;
    GLint LightColor = HANDLE_NULL;
    GLint LightSize = HANDLE_NULL;
    GLint JointMatrix = HANDLE_NULL;
    GLint IsSkinning = HANDLE_NULL;
};

struct GLAttributeHandles {
    GLint Position = HANDLE_NULL;
//    GLint Color = HANDLE_NULL;
    GLint Normal = HANDLE_NULL;
//    GLint DiffuseMaterial = HANDLE_NULL;
    GLint TextureCoord = HANDLE_NULL;
    GLint Weight = HANDLE_NULL;
    GLint JointId = HANDLE_NULL;
};

class GLProgramHandle : public SObject {
public:
    GLProgramHandle();


    ~GLProgramHandle();


    void Exterminate() override;

    void SetProgram(GLuint program) {
        Program = program;
    }

    GLint AttributeLocation(const char* location) const {
        auto pair = AttributesList.find(location);
        return pair == AttributesList.end() ? -1 : pair->second;
    }

    GLint UniformLocation(const char* location) const {
        auto pair = UniformsList.find(location);
        return pair == UniformsList.end() ? -1 : pair->second;
    }

    void SetAttribVec3(std::string location, vec3& value);

    void SetAttribVec4(std::string location, vec4& value);

    void SetUniformInt(std::string location, int value);

    void SetUniformFloat(std::string location, float value);

    void SetUniformMat4(std::string location, mat4& value);

    void SetUniformMat3(std::string location, mat3& value);


    void GetAttributesList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag);

    void GetUniformsList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag);

private:
    static std::string getImplementName(std::map<std::string, std::string>& list, std::string name);

    static bool isExist(std::string src, std::string target, std::map<std::string, std::string>& variablesList) {
        return src.find(variablesList[target]) != std::string::npos;
    }

public:
    GLuint Program;
    GLAttributeHandles Attributes;
    GLUniformHandles Uniforms;

    std::map<std::string, int> AttributesList;
    std::map<std::string, int> UniformsList;
};
