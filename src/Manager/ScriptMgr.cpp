#include "ScriptMgr.h"

#include <sstream>
#include "sqrat.h"

#include "../Util/AssetsDef.h"
#ifdef WIN32
#include <windows.h>
#endif

using namespace SqPlus;
using namespace CSE;




ScriptMgr::ScriptMgr() {}


ScriptMgr::~ScriptMgr() {
	SquirrelVM::Shutdown();

}


void ScriptMgr::Init() const {

	Sqrat::DefaultVM::Set(vm);
	//SquirrelVM::Init();
	DefineClasses();
	ReadScriptList();
}


void ScriptMgr::RegisterScriptInAsset(std::string path) {

	std::string script_str = CSE::OpenAssetsTxtFile(CSE::AssetsPath() + path);

	//스크립트의 존재여부 확인
	if (!script_str.empty()) {
		SquirrelObject compiledScript = SquirrelVM::CompileBuffer(script_str.c_str());
		try {
			SquirrelVM::RunScript(compiledScript);
		} catch (SquirrelError & e) {
#ifdef WIN32
			OutputDebugString(e.desc);
#endif
		}
	}

}


void ScriptMgr::DefineClasses() {

	//GameObject

	SQClassDef<SGameObject>(_T("GameObject"))
		.func(&SGameObject::GetTransform, _T("GetTransform"))
		.func(&SGameObject::GetIsEnable, _T("IsEnable"))
		.func(&SGameObject::SetIsEnable, _T("SetEnable"));

	//Components

	SQClassDef<TransformInterface>(_T("TRANSFORM"))
		.var(&TransformInterface::m_position, _T("position"))
		.var(&TransformInterface::m_rotation, _T("rotation"))
		.var(&TransformInterface::m_scale, _T("scale"));

	SQClassDef<CustomComponent>(_T("SCEngineComponent"))
		.func(&CustomComponent::GetGameObject, _T("GetGameObject"))
		.func(&CustomComponent::GetIsEnable, _T("IsEnable"))
		.func(&CustomComponent::SetIsEnable, _T("SetEnable"));

	//Util
	SQClassDef<vec3>(_T("vec3"))
		.var(&vec3::x, _T("x"))
		.var(&vec3::y, _T("y"))
		.var(&vec3::z, _T("z"));

}


void ScriptMgr::ReadScriptList() const {

	std::stringstream list(OpenAssetsTxtFile(AssetsPath() + "Script/core/script_list.ini"));
	std::string path;

	while (std::getline(list, path, '\n')) {
		RegisterScriptInAsset(path);
	}

}
