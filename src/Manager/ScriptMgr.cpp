#include "ScriptMgr.h"

#include <sstream>
#include "sqrat.h"
#include "sqrat/sqratVM.h"

#include "../Util/AssetsDef.h"
#include "../Util/Render/SMaterial.h"
#include "../Component/LightComponent.h"
#include "../Object/SScriptObject.h"
#include "../Component/RenderComponent.h"
#include "EngineCore.h"

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

const char* CSEngineScript = " 				  \n\
class CSEngineScript {						  \n\
//CSEngineComponent							  \n\
CSEngine = null;							  \n\
gameObject = null;							  \n\
gameobject = null;							  \n\
											  \n\
function SetCSEngine(component) {			  \n\
    if(CSEngine != null) return;              \n\
	CSEngine = component;					  \n\
	gameobject = CSEngine.GetGameObject();	  \n\
	gameObject = CSEngine.GetGameObject();	  \n\
}											  \n\
function IsEnable(){						  \n\
return CSEngine.IsEnable();					  \n\
}											  \n\
											  \n\
function SetEnable(enable) {				  \n\
	CSEngine.SetEnable(enable);				  \n\
}											  \n\
											  \n\
function GetTransform() {					  \n\
	return gameobject.GetTransform();		  \n\
}											  \n\
function Log(log){							  \n\
CSEngine.Log(log);							  \n\
}											  \n\
											  \n\
											  \n\
}";

#define COMPONENT_DEF(CLASSNAME) SQRComponentDef<CLASSNAME>(_SC(#CLASSNAME))
#define COMPONENT_DEF_WITH_SQNAME(CLASSNAME, SQNAME) SQRComponentDef<CLASSNAME>(_SC(#SQNAME))

ScriptMgr::ScriptMgr() = default;


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


void ScriptMgr::RegisterScript(const std::string& script) {

    HSQUIRRELVM vm = DefaultVM::Get();

    //register script
    if (!script.empty()) {
        Script compiledScript;
        compiledScript.CompileString(script);
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

    if (customComponent._type == OT_NULL) {
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
            .SquirrelFunc(_SC("GetClass"), GetCustomComponentFunc);
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


    COMPONENT_DEF_WITH_SQNAME(CustomComponent, CSEngineComponent)
            .Func(_SC("GetGameObject"), &CustomComponent::GetGameObject)
            .Func(_SC("IsEnable"), &CustomComponent::GetIsEnable)
            .Func(_SC("SetEnable"), &CustomComponent::SetIsEnable)
            .Func(_SC("Log"), &CustomComponent::Log);

    COMPONENT_DEF(RenderComponent)
            .Func(_SC("GetMaterial"), &RenderComponent::GetMaterial)
            .Func(_SC("SetEnable"), &RenderComponent::SetIsEnable)
            .Func(_SC("GetEnable"), &RenderComponent::GetIsEnable);


    COMPONENT_DEF(LightComponent)
            .Func(_SC("SetLightType"), &LightComponent::SetLightType)
            .Func(_SC("SetColor"), &LightComponent::SetColor)
            .Func(_SC("GetColor"), &LightComponent::GetColor)
            .Func(_SC("SetDirection"), &LightComponent::SetDirection)
            .Func(_SC("GetDirection"), &LightComponent::GetDirection);

    COMPONENT_DEF(CameraComponent)
            .Func(_SC("SetTarget"), &CameraComponent::SetTarget)
            .Func(_SC("SetTargetVector"), &CameraComponent::SetTargetVector)
            .Func(_SC("SetUp"), &CameraComponent::SetUp)
            .Func(_SC("SetOrtho"), &CameraComponent::SetOrtho)
            .Func(_SC("SetZDepthRange"), &CameraComponent::SetZDepthRange)
            .Func(_SC("SetPerspective"), &CameraComponent::SetPerspective);


    //Util

    SQRClassDef<vec2>(_SC("vec2"))
            .Var(_SC("x"), &vec2::x)
            .Var(_SC("y"), &vec2::y)
            .Func(_SC("Set"), &vec2::Set);

    SQRClassDef<vec3>(_SC("vec3"))
            .Var(_SC("x"), &vec3::x)
            .Var(_SC("y"), &vec3::y)
            .Var(_SC("z"), &vec3::z)
            .Func(_SC("Cross"), &vec3::Cross)
            .Func(_SC("Dot"), &vec3::Dot)
            .Func(_SC("Set"), &vec3::Set);

    SQRClassDef<vec4>(_SC("vec4"))
            .Var(_SC("x"), &vec4::x)
            .Var(_SC("y"), &vec4::y)
            .Var(_SC("z"), &vec4::z)
            .Var(_SC("w"), &vec4::w)
            .Func(_SC("Set"), &vec4::Set);

    SQRClassDef<Quaternion>(_SC("Quaternion"))
            .Var(_SC("x"), &Quaternion::x)
            .Var(_SC("y"), &Quaternion::y)
            .Var(_SC("z"), &Quaternion::z)
            .Var(_SC("w"), &Quaternion::w)
            .Func(_SC("Set"), &Quaternion::Set)
            .Func(_SC("Clone"), &Quaternion::Clone)
            .StaticFunc(_SC("AngleAxis"), &Quaternion::AngleAxis)
            .Func(_SC("Rotate"), &Quaternion::Rotate)
            .Func(_SC("Slerp"), &Quaternion::Slerp)
            .Func(_SC("ToEulerAngle"), &Quaternion::ToEulerAngle);

    SQRClassDef<SMaterial>(_SC("Material"))
            .Func(_SC("SetInt"), &SMaterial::SetInt)
            .Func(_SC("SetFloat"), &SMaterial::SetFloat)
            .Func(_SC("SetVec3"), &SMaterial::SetVec3)
            .Func(_SC("SetTexture"), &SMaterial::SetTexture);
}


void ScriptMgr::ReleaseSqratObject() {

    for (auto obj : m_objects) {
        obj->Release();
        SAFE_DELETE(obj);
    }

}


void ScriptMgr::ReadScriptList() {
    auto assets = CORE->GetCore(ResMgr)->GetAssetReferences(AssetMgr::TYPE::SCRIPT);

    //compile base script class
    RegisterScript(CSEngineScript);

    for (const auto& asset : assets) {
        SResource::Create<SScriptObject>(asset);
    }

}
