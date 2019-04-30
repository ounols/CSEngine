#pragma once
#include "../OGLDef.h"
#include "../SObject.h"
#include "../Manager/ResMgr.h"
#include "../Manager/ShaderProgramContainer.h"

#define HANDLE_NULL -1
#define MAX_LIGHTS 16
#define MAX_JOINTS 60

struct GLUniformHandles {
	GLint Modelview = HANDLE_NULL;
	GLint ModelNoCameraMatrix = HANDLE_NULL;
	GLint Projection = HANDLE_NULL;
	GLint NormalMatrix = HANDLE_NULL;
	GLint LightPosition[MAX_LIGHTS] = { HANDLE_NULL };
	GLint AmbientMaterial = HANDLE_NULL;
	GLint SpecularMaterial = HANDLE_NULL;
	GLint Shininess = HANDLE_NULL;
	GLint TextureSampler2D = HANDLE_NULL;
	GLint DiffuseLight[MAX_LIGHTS] = { HANDLE_NULL };
	GLint AmbientLight[MAX_LIGHTS] = { HANDLE_NULL };
	GLint SpecularLight[MAX_LIGHTS] = { HANDLE_NULL };
	GLint LightMode[MAX_LIGHTS] = { HANDLE_NULL };
	GLint Interpolation_z = HANDLE_NULL;
	GLint AttenuationFactor[MAX_LIGHTS] = { HANDLE_NULL };
	GLint IsAttenuation[MAX_LIGHTS] = { HANDLE_NULL };
	GLint LightRadius[MAX_LIGHTS] = { HANDLE_NULL };
	GLint SpotDirection[MAX_LIGHTS] = { HANDLE_NULL };
	GLint SpotExponent[MAX_LIGHTS] = { HANDLE_NULL };
	GLint SpotCutOffAngle[MAX_LIGHTS] = { HANDLE_NULL };
	GLint IsDirectional[MAX_LIGHTS] = { HANDLE_NULL };
	GLint LightsSize = HANDLE_NULL;
	GLint JointMatrix = { HANDLE_NULL };
	GLint IsSkinning = HANDLE_NULL;
};

struct GLAttributeHandles {
	GLint Position = HANDLE_NULL;
	GLint Color = HANDLE_NULL;
	GLint Normal = HANDLE_NULL;
	GLint DiffuseMaterial = HANDLE_NULL;
	GLint TextureCoord = HANDLE_NULL;
	GLint Weight = HANDLE_NULL;
	GLint JointId = HANDLE_NULL;
};

class GLProgramHandle : public SObject {
public:
	GLProgramHandle() : Program(HANDLE_NULL) {
		ResMgr::getInstance()->Register<ShaderProgramContainer, GLProgramHandle>(this);
		SetUndestroyable(true);
	}


	~GLProgramHandle() {
	}


	void Exterminate() override {
	}

	void AttributeLocation(GLint& target, const char* location) {
		glUseProgram(Program);

		target = glGetAttribLocation(Program, location);
	}

	void UniformLocation(GLint& target, const char* location) {
		glUseProgram(Program);

		target = glGetUniformLocation(Program, location);
	}


	GLuint Program;
	GLAttributeHandles Attributes;
	GLUniformHandles Uniforms;
};
