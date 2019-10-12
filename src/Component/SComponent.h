#pragma once

#include "../SObject.h"
#include <string>
#include <map>
#include "../Object/SGameObject.h"
#include "SISComponent.h"
#include "../Util/ComponentDef.h"
#include "../Util/VariableBinder.h"


namespace CSE {

    class SGameObject;

    class SComponent : public SObject, public virtual SISComponent, public VariableBinder {
    public:

        explicit SComponent(std::string classType) : m_classType(classType) {

        }

        SComponent(const SComponent& src) : SISComponent(src) {
            gameObject = src.gameObject;
            isEnable = src.isEnable;
            m_classType = src.m_classType;
        }


        virtual ~SComponent() {

        }

        virtual void Start() override {
            return;
        }

        virtual SComponent* Clone(SGameObject* object) {
            return nullptr;
        }

        virtual void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                   std::map<SComponent*, SComponent*> lists_comp) {
            return;
        }

        void SetValue(std::string name_str, Arguments value) override {}

        std::string PrintValue() const override { return std::string(); }

        void SetGameObject(SGameObject* object) {
            gameObject = object;
        }

        virtual SGameObject* GetGameObject() const {
            return gameObject;
        }


        virtual bool GetIsEnable() const {
            return isEnable;
        }


        virtual void SetIsEnable(bool is_enable) {
            isEnable = is_enable;
        }

        std::string GetClassType() const {
            return m_classType;
        }

        void SetClassType(std::string type) {
            m_classType = type;
        }

    protected:
        SGameObject* gameObject = nullptr;
        bool isEnable = true;

        std::string m_classType;

    };

}