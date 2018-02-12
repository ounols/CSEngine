#include "ScriptMgr.h"

#include <sstream>
#include "sqrat.h"

#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include "../Component/MaterialComponent.h"
#include "../Component/LightComponent.h"
#ifdef WIN32
#include <windows.h>
#endif

using namespace Sqrat;
using namespace CSE;

const char* SCEngineScript = " 				  \n\
class SCEngineScript {						  \n\
//SCEngineComponent							  \n\
SCEngine = null;							  \n\
gameObject = null;							  \n\
gameobject = null;							  \n\
											  \n\
function SetSCEngine(component) {			  \n\
	SCEngine = component;					  \n\
	gameobject = SCEngine.GetGameObject();	  \n\
	gameObject = SCEngine.GetGameObject();	  \n\
}											  \n\
function IsEnable(){						  \n\
return SCEngine.IsEnable();					  \n\
}											  \n\
											  \n\
function SetEnable(enable) {				  \n\
	SCEngine.SetEnable(enable);				  \n\
}											  \n\
function Log(log){							  \n\
SCEngine.Log(log);							  \n\
}											  \n\
											  \n\
											  \n\
}";

#define COMPONENT_DEF(CLASSNAME) SQRComponentDef<CLASSNAME>(_SC(#CLASSNAME))
#define COMPONENT_DEF_WITH_SQNAME(CLASSNAME, SQNAME) SQRComponentDef<CLASSNAME>(_SC(#SQNAME))

ScriptMgr::ScriptMgr() {}


ScriptMgr::~ScriptMgr() {
	ReleaseSqratObject();
	sq_close(DefaultVM::Get());
}


void ScriptMgr::Init() {
	HSQUIRRELVM vm;
	vm = sq_open(1024);
	Sqrat::DefaultVM::Set(vm);
	//SquirrelVM::Init();
	DefineClasses();
	ReadScriptList();
}


void ScriptMgr::RegisterScriptInAsset(std::string path) {

	std::string script_str = CSE::OpenAssetsTxtFile(CSE::AssetsPath() + path);

	//replace GetComponent function
	script_str = ReplaceAll(script_str, "GetComponent<", "GetComponent_");
	script_str = ReplaceAll(script_str, ">()", "_()");

	RegisterScript(script_str);

}


void ScriptMgr::RegisterScript(std::string script) {

	HSQUIRRELVM vm = DefaultVM::Get();

	//스크립트의 존재여부 확인
	if (!script.empty()) {
		Script compiledScript;
		compiledScript.CompileString(script.c_str());
		if (Sqrat::Error::Occurred(vm)) {
#ifdef WIN32
			OutputDebugString(_SC("Compile Failed: "));
			OutputDebugString(Error::Message(vm).c_str());
#endif
		}

		compiledScript.Run();
		if (Sqrat::Error::Occurred(vm)) {
#ifdef WIN32
			OutputDebugString(_SC("Run Failed: "));
			OutputDebugString(Error::Message(vm).c_str());
#endif
		}

		compiledScript.Release();
	}

}


void ScriptMgr::DefineClasses() {

	HSQUIRRELVM vm = DefaultVM::Get();

	//GameObject

	SQRClassDef<SGameObject>(_SC("GameObject"))
		.Func(_SC("GetTransform"), &SGameObject::GetTransform)
		.Func(_SC("IsEnable"), &SGameObject::GetIsEnable)
		.Func(_SC("SetEnable"), &SGameObject::SetIsEnable)
	;
	//Components

	/**	Default
	SQRComponentDef<Default>(_SC("Default"))
		.Func(_SC("GetGameObject"), &Default::GetGameObject)
		.Func(_SC("IsEnable"), &Default::GetIsEnable)
		.Func(_SC("SetEnable"), &Default::SetIsEnable)
	;
	**/

	SQRClassDef<TransformInterface>(_SC("Transform"))
		.Var(_SC("position"), &TransformInterface::m_position)
		.Var(_SC("rotation"), &TransformInterface::m_rotation)
		.Var(_SC("scale"), &TransformInterface::m_scale);


	COMPONENT_DEF_WITH_SQNAME(CustomComponent, SCEngineComponent)
		.Func(_SC("GetGameObject"), &CustomComponent::GetGameObject)
		.Func(_SC("IsEnable"), &CustomComponent::GetIsEnable)
		.Func(_SC("SetEnable"), &CustomComponent::SetIsEnable)
		.Func(_SC("Log"), &CustomComponent::Log)
	;

	COMPONENT_DEF(MaterialComponent)
		.Func(_SC("SetAmbient"), &MaterialComponent::SetMaterialAmbient)
		.Func(_SC("SetSpecular"), &MaterialComponent::SetMaterialSpecular)
		.Func(_SC("SetDiffuse"), &MaterialComponent::SetDiffuseMaterial)
		.Func(_SC("SetShininess"), &MaterialComponent::SetShininess);


	COMPONENT_DEF(LightComponent)
		.Func(_SC("SetLightType"), &LightComponent::SetLightType)
		.Func(_SC("SetColorAmbient"), &LightComponent::SetColorAmbient)
		.Func(_SC("SetColorDiffuse"), &LightComponent::SetColorDiffuse)
		.Func(_SC("SetColorSpecular"), &LightComponent::SetColorSpecular)
	;


	//Util

	SQRClassDef<vec3>(_SC("vec3"))
		.Var(_SC("x"), &vec3::x)
		.Var(_SC("y"), &vec3::y)
		.Var(_SC("z"), &vec3::z);

	SQRClassDef<vec4>(_SC("vec4"))
		.Var(_SC("x"), &vec4::x)
		.Var(_SC("y"), &vec4::y)
		.Var(_SC("z"), &vec4::z)
		.Var(_SC("w"), &vec4::w);

}


void ScriptMgr::ReleaseSqratObject() {

	for(auto obj : m_objects) {
		obj->Release();
		SAFE_DELETE(obj);
	}

}


void ScriptMgr::ReadScriptList() const {

	std::stringstream list(OpenAssetsTxtFile(AssetsPath() + "Script/core/script_list.ini"));
	std::string path;

	//compile base script class
	RegisterScript(SCEngineScript);

	while (std::getline(list, path, '\n')) {
		RegisterScriptInAsset(path);
	}

}
