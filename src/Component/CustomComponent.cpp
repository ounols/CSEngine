#include "CustomComponent.h"
#include "../Util/AssetsDef.h"
#include "../Manager/ScriptMgr.h"
#ifdef WIN32
#include <windows.h>
#elif __ANDROID__
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,__VA_ARGS__)
#include <android/log.h>
#endif

using namespace CSE;


COMPONENT_CONSTRUCTOR(CustomComponent) {
}


CustomComponent::~CustomComponent() {

}


void CustomComponent::Exterminate() {
	SAFE_DELETE(m_specialization);
	SAFE_DELETE(m_classInstance);
}


void CustomComponent::Init() {

	if (m_specialization == nullptr) return;
	if (m_classInstance != nullptr) {
		SAFE_DELETE(m_classInstance);
	}

	m_classInstance = m_specialization->NewPointer();

	if (m_funcSetCSEngine < 0) return;
	m_classInstance->call(m_funcSetCSEngine, this);

	if (m_funcInit < 0) return;

	try {
		m_classInstance->call(m_funcInit);
	} catch (Sqrat::Exception e) {
		m_isError = true;
#ifdef WIN32
		OutputDebugString(Sqrat::LastErrorString(Sqrat::DefaultVM::Get()).c_str());
#endif
	}

}


void CustomComponent::Tick(float elapsedTime) {

	if (m_specialization == nullptr) return;
	if (m_classInstance == nullptr) return;
	if (m_funcTick < 0) return;
	if (m_isError) return;

	try {
		m_classInstance->call(m_funcTick, elapsedTime);
	}
	catch (Sqrat::Exception e) {
		m_isError = true;
#ifdef WIN32
		OutputDebugString(Sqrat::LastErrorString(Sqrat::DefaultVM::Get()).c_str());
#endif
	}

}


void CustomComponent::RegisterScript() {

	if (m_className.empty()) return;
	if (m_specialization != nullptr)
		SAFE_DELETE(m_specialization);

	try {
		m_specialization = new sqext::SQIClass(m_className.c_str());
		m_specialization->bind(m_funcSetCSEngine, "SetSCEngine");
	}
	catch (Sqrat::Exception e) {
		m_funcSetCSEngine = -1;
#ifdef WIN32
		OutputDebugString(Sqrat::LastErrorString(Sqrat::DefaultVM::Get()).c_str());
#endif
	}

	try {
		m_specialization->bind(m_funcInit, "Init");
	}
	catch (Sqrat::Exception e) { m_funcInit = -1; }
	try {
		m_specialization->bind(m_funcTick, "Tick");
	}
	catch (Sqrat::Exception e) { m_funcTick = -1; }

	try {
		m_specialization->bind(m_funcExterminate, "Destroy");
	}
	catch (Sqrat::Exception e) { m_funcExterminate = -1; }

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


void CustomComponent::Log(const char* log) {
#ifdef WIN32
	std::string log_str = "[Log/";
	log_str.append(m_className + "] : " + log + '\n');
	OutputDebugString(log_str.c_str());
#elif __ANDROID__
	LOGD(m_className.c_str(), log);
#endif

}
