#ifndef CSE_GLOBAL_SCRIPT_DISABLED
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
#include "InputMgr.h"

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
    if (m_vm != nullptr) {
        sq_close(m_vm);
        m_vm = nullptr;
    }
}


void ScriptMgr::Init() {
    if (m_vm != nullptr) {
        return;
    }
//	Sqrat::SqratVM vm = Sqrat::SqratVM();
    m_vm = sq_open(1024);
    Sqrat::DefaultVM::Set(m_vm);
//	Sqrat::DefaultVM::Set(vm.GetVM());
    sq_pushroottable(m_vm);
    sqstd_register_mathlib(m_vm);
    sq_pop(m_vm, 1);
    //SquirrelVM::Init();
    DefineClasses(m_vm);
    ReadScriptList(m_vm);
}


void ScriptMgr::RegisterScript(const std::string& script, HSQUIRRELVM vm) {
    //register script
    if (!script.empty()) {
        Script compiledScript;
        compiledScript.CompileString(script);
        if (Sqrat::Error::Occurred(vm)) {
            SafeLog::LogErr((_SC("Compile Failed: ") + Error::Message(vm)).c_str());
        }

        compiledScript.Run();
        if (Sqrat::Error::Occurred(vm)) {
            SafeLog::LogErr((_SC("Run Failed: ") + Error::Message(vm)).c_str());
        }

        compiledScript.Release();
    }

}

const HSQUIRRELVM & ScriptMgr::GetVM() const {
    return m_vm;
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
            .Func(_SC("Set"), &vec3::Set)
            .Func(_SC("Distance"), static_cast<float(vec3::*)(const vec3&) const>(&vec3::Distance))
            .Func(_SC("DistanceSquared"), &vec3::DistanceSquared);

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

    //InputMgr - Input system for keyboard and mouse
    SQRClassDef<InputMgr>(_SC("Input"), vm)
            .StaticFunc(_SC("GetKey"), &InputMgr::GetKey)
            .StaticFunc(_SC("GetKeyDown"), &InputMgr::GetKeyDown)
            .StaticFunc(_SC("GetKeyUp"), &InputMgr::GetKeyUp)
            .StaticFunc(_SC("GetMouseButton"), &InputMgr::GetMouseButton)
            .StaticFunc(_SC("GetMouseButtonDown"), &InputMgr::GetMouseButtonDown)
            .StaticFunc(_SC("GetMouseButtonUp"), &InputMgr::GetMouseButtonUp)
            .StaticFunc(_SC("GetMousePosition"), &InputMgr::GetMousePosition);

    // KeyCode enum
    Enumeration keyCodeEnum(vm);
    keyCodeEnum.Const(_SC("A"), static_cast<int>(KeyCode::A));
    keyCodeEnum.Const(_SC("D"), static_cast<int>(KeyCode::D));
    keyCodeEnum.Const(_SC("W"), static_cast<int>(KeyCode::W));
    keyCodeEnum.Const(_SC("S"), static_cast<int>(KeyCode::S));
    keyCodeEnum.Const(_SC("Left"), static_cast<int>(KeyCode::Left));
    keyCodeEnum.Const(_SC("Right"), static_cast<int>(KeyCode::Right));
    keyCodeEnum.Const(_SC("Up"), static_cast<int>(KeyCode::Up));
    keyCodeEnum.Const(_SC("Down"), static_cast<int>(KeyCode::Down));
    keyCodeEnum.Const(_SC("Space"), static_cast<int>(KeyCode::Space));
    keyCodeEnum.Const(_SC("Escape"), static_cast<int>(KeyCode::Escape));
    RootTable(vm).Bind(_SC("KeyCode"), keyCodeEnum);

    // MouseButton enum
    Enumeration mouseButtonEnum(vm);
    mouseButtonEnum.Const(_SC("Left"), static_cast<int>(MouseButton::Left));
    mouseButtonEnum.Const(_SC("Right"), static_cast<int>(MouseButton::Right));
    mouseButtonEnum.Const(_SC("Middle"), static_cast<int>(MouseButton::Middle));
    RootTable(vm).Bind(_SC("MouseButton"), mouseButtonEnum);
}


void ScriptMgr::ReleaseSqratObject() {

    for (auto obj : m_objects) {
        obj->Release();
        SAFE_DELETE(obj);
    }

}


void ScriptMgr::ReadScriptList(HSQUIRRELVM vm) {
    auto assets = CORE->GetCore(ResMgr)->GetAssetReferences(AssetMgr::TYPE::SCRIPT);

    //compile base script class
    RegisterScript(CSEngineScript, vm);

    for (const auto& asset : assets) {
        SResource::Create<SScriptObject>(asset);
    }

}
#endif