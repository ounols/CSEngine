#pragma once
#include "../OGLDef.h"
#include "../SObject.h"
#include "../Manager/ResMgr.h"

#define HANDLE_NULL -1

struct GLUniformHandles {
	GLuint Modelview = HANDLE_NULL;
	GLuint Projection = HANDLE_NULL;
	GLuint NormalMatrix = HANDLE_NULL;
	GLuint LightPosition = HANDLE_NULL;
	GLint AmbientMaterial = HANDLE_NULL;
	GLint SpecularMaterial = HANDLE_NULL;
	GLint Shininess = HANDLE_NULL;
	GLint TextureMode = HANDLE_NULL;
};

struct GLAttributeHandles {
	GLint Position = HANDLE_NULL;
	GLint Color = HANDLE_NULL;
	GLint Normal = HANDLE_NULL;
	GLint DiffuseMaterial = HANDLE_NULL;
	GLint TextureCoord = HANDLE_NULL;
};

class GLProgramHandle :
	public SObject
{
public:
	GLProgramHandle() : Program(HANDLE_NULL) {
		ResMgr::getInstance()->RegisterProgram(this);
		SetUndestroyable(true);
	}
	~GLProgramHandle() {}
	void Exterminate() override {}

	GLuint Program;
	GLAttributeHandles Attributes;
	GLUniformHandles Uniforms;
};