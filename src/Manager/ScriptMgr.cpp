#include "ScriptMgr.h"

#include <sstream>
#include "sqrat.h"
#include "sqrat/sqratVM.h"

#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include "../Component/MaterialComponent.h"
#include "../Component/LightComponent.h"
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __ANDROID__
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"ScriptManager",__VA_ARGS__)
#elif __linux__
#include <iostream>
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
											  \n\
function GetTransform() {					  \n\
	return gameobject.GetTransform();		  \n\
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
//	Sqrat::SqratVM vm = Sqrat::SqratVM();
	vm = sq_open(1024);
	Sqrat::DefaultVM::Set(vm);
//	Sqrat::DefaultVM::Set(vm.GetVM());
	sq_pushroottable(vm);
	sqstd_register_mathlib(vm);
	sq_pop(vm, 1);
	//SquirrelVM::Init();
	DefineClasses();
	ReadScriptList();
}


void ScriptMgr::RegisterScriptInAsset(std::string path) {

	std::string script_str = CSE::OpenAssetsTxtFile(CSE::AssetsPath() + path);

	//replace GetComponent function
	//script_str = ReplaceAll(script_str, "GetComponent<", "GetComponent_");
	//script_str = ReplaceAll(script_str, ">()", "_()");
	script_str = ReplaceFunction(script_str, "GetComponent<", ">()", "GetComponent_", "_()");

	//replace GetCustomComponent(GetClass) function
	script_str = ReplaceFunction(script_str, "GetClass<", ">()", "GetClass(\"", "\")");

	RegisterScript(script_str);

}


void ScriptMgr::RegisterScript(std::string script) {

	HSQUIRRELVM vm = DefaultVM::Get();

	//register script
	if (!script.empty()) {
		Script compiledScript;
		compiledScript.CompileString(script.c_str());
		if (Sqrat::Error::Occurred(vm)) {
#ifdef WIN32
			OutputDebugString(_SC("Compile Failed: "));
			OutputDebugString(Error::Message(vm).c_str());
#elif __ANDROID__
            LOGE("Compile Failed : %s", Error::Message(vm).c_str());
#elif __linux__
			std::cout << "Compile Failed : " << Error::Message(vm) << '\n';
#endif
		}

		compiledScript.Run();
		if (Sqrat::Error::Occurred(vm)) {
#ifdef WIN32
			OutputDebugString(_SC("Run Failed: "));
			OutputDebugString(Error::Message(vm).c_str());
#elif __ANDROID__
            LOGE("Run Failed : %s", Error::Message(vm).c_str());
#elif __linux__
			std::cout << "Run Failed : " << Error::Message(vm) << '\n';
#endif
		}

		compiledScript.Release();
	}

}

SQInteger GetCustomComponentFunc(HSQUIRRELVM v) {
	SQInteger args = sq_gettop(v);

	if (args != 2) return 1;

	SGameObject* game_object = Var<SGameObject*>(v, 1).value;
	char* classname_str = Var<char*>(v, 2).value;
	auto customComponent = game_object->GetCustomComponent(classname_str);

	if(customComponent._type == OT_NULL) {
		return 1;
	}

	sq_pushobject(v, customComponent);

	return 1;

	

}


void ScriptMgr::DefineClasses(HSQUIRRELVM vm) {

	

	//GameObject

	SQRClassDef<SGameObject>(_SC("GameObject"), vm)
		.Func(_SC("Find"), &SGameObject::Find)
		.Func(_SC("GetTransform"), &SGameObject::GetTransform)
		.Func(_SC("IsEnable"), &SGameObject::GetIsEnable)
		.Func(_SC("SetEnable"), &SGameObject::SetIsEnable)
		.Func(_SC("SetName"), &SGameObject::SetName)
		.Func(_SC("GetName"), &SGameObject::GetName)
		.SquirrelFunc(_SC("GetClass"), GetCustomComponentFunc)
	;
	//Components

	/**	Default
	SQRComponentDef<Default>(_SC("Default"))
		.Func(_SC("GetGameObject"), &Default::GetGameObject)
		.Func(_SC("IsEnable"), &Default::GetIsEnable)
		.Func(_SC("SetEnable"), &Default::SetIsEnable)
	;
	**/

	

	SQRClassDef<TransformInterface>(_SC("Transform"), vm)
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
		.Func(_SC("GetColorAmbient"), &LightComponent::GetColorAmbient)
		.Func(_SC("GetColorDiffuse"), &LightComponent::GetColorDiffuse)
		.Func(_SC("GetColorSpecular"), &LightComponent::GetColorSpecular)
		.Func(_SC("SetDirection"), &LightComponent::SetDirection)
		.Func(_SC("GetDirection"), &LightComponent::GetDirection)
	;


	//Util

	SQRClassDef<vec2>(_SC("vec2"))
		.Var(_SC("x"), &vec2::x)
		.Var(_SC("y"), &vec2::y)
		.Func(_SC("Set"), &vec2::Set)
	;

	SQRClassDef<vec3>(_SC("vec3"))
		.Var(_SC("x"), &vec3::x)
		.Var(_SC("y"), &vec3::y)
		.Var(_SC("z"), &vec3::z)
		.Func(_SC("Set"), &vec3::Set)
	;

	SQRClassDef<vec4>(_SC("vec4"))
		.Var(_SC("x"), &vec4::x)
		.Var(_SC("y"), &vec4::y)
		.Var(_SC("z"), &vec4::z)
		.Var(_SC("w"), &vec4::w)
		.Func(_SC("Set"), &vec4::Set)
	;

	SQRClassDef<Quaternion>(_SC("Quaternion"))
	.Var(_SC("x"), &Quaternion::x)
	.Var(_SC("y"), &Quaternion::y)
	.Var(_SC("z"), &Quaternion::z)
	.Var(_SC("w"), &Quaternion::w)
	.StaticFunc(_SC("AngleAxis"), &Quaternion::AngleAxis)
	.Func(_SC("Rotate"), &Quaternion::Rotate)
	.Func(_SC("ToEulerAngle"), &Quaternion::ToEulerAngle)
	;

}


void ScriptMgr::DefineScenes(HSQUIRRELVM vm) {


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
        path = ReplaceAll(path, "\r", "");
		RegisterScriptInAsset(path);
	}

}
