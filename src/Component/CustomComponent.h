#pragma once

#include "SComponent.h"
#include "sqext.h"

#include <map>

namespace CSE {

    class CustomComponent : public SComponent {

    private:
        struct VARIABLE {

            VARIABLE(std::string name, const char* type, std::string value) {
                this->name = name;
                this->type = type;
                this->value = value;
            }

            std::string name;
            const char* type;
            std::string value;
        };
    public:
        CustomComponent();

        ~CustomComponent();


        void Exterminate() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        SComponent* Clone(SGameObject* object) override;

        void SetClassName(std::string name);

        std::string SGetClassName() const;


        bool GetIsEnable() const override;

        void SetIsEnable(bool is_enable) override;

        void Log(const char* log);

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

        SGameObject* GetGameObject() const override;


        Sqrat::Object GetClassInstance() const {
            return m_classInstance->get();
        }
//
//        auto GetComponent() -> CustomComponent* override {
//            return this;
//        }

    private:
        void RegisterScript();
        void CreateClassInstance(std::vector<std::string> variables);

        void BindValue(VARIABLE* variable, const char* value) const;

    private:
        mutable sqext::SQIClass* m_specialization = nullptr;
        mutable sqext::SQIClassInstance* m_classInstance = nullptr;
        int m_funcSetCSEngine = 0;
        int m_funcInit = 1;
        int m_funcTick = 2;
        int m_funcExterminate = 3;
        std::vector<VARIABLE> m_variables;

        std::string m_className = "";
        std::string m_classID = "";

        bool m_isError = false;
    };
}