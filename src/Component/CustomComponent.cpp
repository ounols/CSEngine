#include "CustomComponent.h"
#include "../Util/AssetsDef.h"
#include "../Manager/ScriptMgr.h"
#ifdef WIN32
#include <windows.h>
#endif

using namespace CSE;

CustomComponent::CustomComponent() {
}


CustomComponent::~CustomComponent() {
	
}


void CustomComponent::Exterminate() {

	if (m_specialization.IsNull()) return;
	if (m_funcExterminate.func.IsNull()) return;

	m_funcExterminate();

}


void CustomComponent::Init() {

	if (m_specialization.IsNull()) return;

	m_classInstance = SquirrelVM::CreateInstance(m_specialization);

	if (m_classInstance.IsNull()) return;

	m_funcTick = SqPlus::SquirrelFunction<void>(m_classInstance, "Tick");
	m_funcExterminate = SqPlus::SquirrelFunction<void>(m_classInstance, "Destroy");

	SqPlus::SquirrelFunction<void> funcSetSCEngine(m_classInstance, "SetSCEngine");
	if (funcSetSCEngine.func.IsNull()) return;

	try {
		funcSetSCEngine(this);
	}
	catch (SquirrelError & e) {
#ifdef WIN32
		OutputDebugString(e.desc);
#endif
	}

	SqPlus::SquirrelFunction<void> funcInit(m_classInstance, "Init");

	if (funcInit.func.IsNull()) return;

	funcInit();

}


void CustomComponent::Tick(float elapsedTime) {

	if (m_specialization.IsNull()) return;
	if (m_classInstance.IsNull()) return;
	if (m_funcTick.func.IsNull()) return;

	try {
		m_funcTick(elapsedTime);
	} catch (SquirrelError & e) {
#ifdef WIN32
		OutputDebugString(e.desc);
#endif
	}
	

}




void CustomComponent::RegisterScript() const {

	if (m_className.empty()) return;

	m_specialization = SquirrelVM::GetRootTable().GetValue(m_className.c_str());

}


void CustomComponent::SetClassName(std::string name) {
	m_className = name;
	RegisterScript();
	Init();
}


bool CustomComponent::GetIsEnable() const {
	return isEnable;
}


void CustomComponent::SetIsEnable(bool is_enable) {
	isEnable = is_enable;
}
