#pragma once
#include "../Component/CustomComponent.h"
#include "../Util/Matrix.h"

class ScriptMgr : public SContainer<Sqrat::Object*> {
public:
	ScriptMgr();
	~ScriptMgr();

	void Init();


	static void RegisterScriptInAsset(std::string path);
	static void RegisterScript(std::string script);

private:
	void DefineClasses();
	void ReleaseSqratObject();
	void ReadScriptList() const;

	template<class T>
	Sqrat::Class<T>& SQRClassDef(std::string className);
	template<class T>
	Sqrat::Class<T>& SQRComponentDef(std::string className);

private:
	int m_gameobjectIndex = -1;
};


template <class T>
Sqrat::Class<T>& ScriptMgr::SQRClassDef(std::string className) {

	Sqrat::Class<T>* sqClass = new Sqrat::Class<T>(Sqrat::DefaultVM::Get(), className);
	Sqrat::RootTable().Bind(className.c_str(), *sqClass);
	Register(sqClass);

	//게임 오브젝트인가?
	if (className == _SC("GameObject")) {
		m_gameobjectIndex = m_objects.size();
	}

	return *sqClass;
}


template <class T>
Sqrat::Class<T>& ScriptMgr::SQRComponentDef(std::string className) {
	if (m_gameobjectIndex >= 0) {
		auto sqClass = *static_cast<Sqrat::Class<SGameObject>*>(Get(m_gameobjectIndex));
		sqClass.Func((std::string() + "GetComponent_" + className + "_").c_str(),
			&SGameObject::GetComponent<T>);

	}

	return SQRClassDef<T>(className);
}
