#pragma once
#include "../Component/CustomComponent.h"
#include "../Util/Matrix.h"

class ScriptMgr {
public:
	ScriptMgr();
	~ScriptMgr();

	void Init() const;


	static void RegisterScriptInAsset(std::string path);

private:
	static void DefineClasses();
	void ReadScriptList() const;

private:
	HSQUIRRELVM vm = nullptr;
};

////Gameobject
//DECLARE_INSTANCE_TYPE_NAME(SGameObject, GameObject);
//
////Components
//DECLARE_INSTANCE_TYPE_NAME(TransformInterface, TRANSFORM);
//DECLARE_INSTANCE_TYPE_NAME(CustomComponent, SCEngineComponent);
//
////Util
//DECLARE_INSTANCE_TYPE_NAME(vec4, vec4);
//DECLARE_INSTANCE_TYPE_NAME(vec3, vec3);
//DECLARE_INSTANCE_TYPE_NAME(vec2, vec2);
//DECLARE_INSTANCE_TYPE_NAME(mat2, mat2);
//DECLARE_INSTANCE_TYPE_NAME(mat3, mat3);
//DECLARE_INSTANCE_TYPE_NAME(mat4, mat4);