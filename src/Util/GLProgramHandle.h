#pragma once

#include "../OGLDef.h"
#include "../Object/SResource.h"
#include "../Manager/ResMgr.h"
#include "Matrix.h"

#include <map>

#define HANDLE_NULL -1
#define MAX_LIGHTS 12
#define MAX_JOINTS 60

namespace CSE {

    struct GLUniformHandles {
        GLint Modelview = HANDLE_NULL;
        GLint ModelNoCameraMatrix = HANDLE_NULL;
        GLint Projection = HANDLE_NULL;
        GLint CameraPosition = HANDLE_NULL;
        // Light
        GLint LightPosition = HANDLE_NULL;
        GLint LightType = HANDLE_NULL;
        GLint LightRadius = HANDLE_NULL;
        GLint LightColor = HANDLE_NULL;
        GLint LightShadowMap = HANDLE_NULL;
        GLint LightMatrix = HANDLE_NULL;
        GLint LightShadowMode = HANDLE_NULL;
        GLint LightSize = HANDLE_NULL;
        GLint LightIrradiance = HANDLE_NULL;
        GLint LightPrefilter = HANDLE_NULL;
        GLint LightBrdfLut = HANDLE_NULL;
        // Skinning
        GLint JointMatrix = HANDLE_NULL;
        GLint SkinningMode = HANDLE_NULL;
    };

    struct GLAttributeHandles {
        GLint Position = HANDLE_NULL;
        GLint Color = HANDLE_NULL;
        GLint Normal = HANDLE_NULL;
//    GLint DiffuseMaterial = HANDLE_NULL;
        GLint TextureCoord = HANDLE_NULL;
        GLint Weight = HANDLE_NULL;
        GLint JointId = HANDLE_NULL;
    };

    class GLProgramHandle : public SResource {
    public:
        struct Element {
            GLenum type = 0;
            int id = HANDLE_NULL;
        };

        typedef std::map<std::string, GLProgramHandle::Element*> GLElementList;
    public:
        GLProgramHandle();


        ~GLProgramHandle();


        void Exterminate() override;

        void SetProgram(GLuint program) {
            Program = program;
        }

		Element* AttributeLocation(const char* location) const {
            auto pair = AttributesList.find(location);
            return pair == AttributesList.end() ? nullptr : pair->second;
        }

		Element* UniformLocation(const char* location) const {
            auto pair = UniformsList.find(location);
            return pair == UniformsList.end() ? nullptr : pair->second;
        }

        void SetAttribVec3(const std::string& location, vec3& value);

        void SetAttribVec4(const std::string& location, vec4& value);

        void SetUniformInt(const std::string& location, int value);

        void SetUniformFloat(const std::string& location, float value);

        void SetUniformMat4(const std::string& location, mat4& value);

        void SetUniformMat3(const std::string& location, mat3& value);


        void SetAttributesList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag);

        void SetUniformsList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag);

        GLElementList GetAttributesList() const;

        GLElementList GetUniformsList() const;

        void SaveShader(const std::string& path);

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        static std::string getImplementName(std::map<std::string, std::string>& list, const std::string& name);


    public:
        GLuint Program;
        GLAttributeHandles Attributes;
        GLUniformHandles Uniforms;

        GLElementList AttributesList;
        GLElementList UniformsList;
    private:
        std::string m_fragShaderName;
        std::string m_vertShaderName;
    };
}