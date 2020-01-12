#pragma once
#include "../Component/CustomComponent.h"
#include "../Util/Matrix.h"
#include "Base/SContainer.h"

namespace CSE {

    class ScriptMgr : public SContainer<Sqrat::Object*> {
    public:
        ScriptMgr();

        ~ScriptMgr();

        void Init();


        static void RegisterScript(std::string script);

    private:
        void DefineClasses(HSQUIRRELVM vm = Sqrat::DefaultVM::Get());

        void ReleaseSqratObject();

        void ReadScriptList() const;

        template <class T>
        Sqrat::Class<T>& SQRClassDef(std::string className, HSQUIRRELVM vm = Sqrat::DefaultVM::Get());

        template <class T>
        Sqrat::Class<T>& SQRComponentDef(std::string className, HSQUIRRELVM vm = Sqrat::DefaultVM::Get());

    private:
        int m_gameobjectIndex = -1;
    };


    template <class T>
    Sqrat::Class<T>& ScriptMgr::SQRClassDef(std::string className, HSQUIRRELVM vm) {

        Sqrat::Class<T>* sqClass = new Sqrat::Class<T>(Sqrat::DefaultVM::Get(), className);
        Sqrat::RootTable(vm).Bind(className.c_str(), *sqClass);
        Register(sqClass);

        //���� ������Ʈ�ΰ�?
        if (className == _SC("GameObject")) {
            m_gameobjectIndex = m_objects.size();
        }

        return *sqClass;
    }


    template <class T>
    Sqrat::Class<T>& ScriptMgr::SQRComponentDef(std::string className, HSQUIRRELVM vm) {
        if (m_gameobjectIndex >= 0) {
            auto sqClass = *static_cast<Sqrat::Class<SGameObject>*>(Get(m_gameobjectIndex));
            sqClass.Func((std::string() + "GetComponent_" + className + "_").c_str(),
                         &SGameObject::GetComponent<T>);

        }

        return SQRClassDef<T>(className, vm);
    }

}