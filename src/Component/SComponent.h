#pragma once

#include "../SObject.h"
#include <string>
#include <map>
#include <utility>
#include "../Object/SGameObject.h"
#include "SISComponent.h"
#include "../Util/ComponentDef.h"
#include "../Util/VariableBinder.h"
#include "../Manager/ReflectionMgr.h"
#include "../Object/Base/ReflectionObject.h"


namespace CSE {

    class SGameObject;

    class SComponent : public SObject, public virtual SISComponent, public VariableBinder, public ReflectionObject {
    public:

        explicit SComponent(std::string classType, SGameObject* gameObject) : ReflectionObject(std::move(classType)),
                                                                              gameObject(gameObject) {
        }

        explicit SComponent(SGameObject* gameObject) : gameObject(gameObject) {
        }

        SComponent(const SComponent& src) : SISComponent(src) {
            gameObject = src.gameObject;
            isEnable = src.isEnable;
            SetClassType(src.GetClassType());
        }


        ~SComponent() override = default;

        void Start() override {}

        virtual SComponent* Clone(SGameObject* object) {
            return nullptr;
        }

        virtual void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
                                   std::map<SComponent*, SComponent*> lists_comp) {}

        virtual auto GetComponent() -> SObject* {
            return this;
        }

        void SetValue(std::string name_str, Arguments value) override {}

        std::string PrintValue() const override { return {}; }

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

    protected:
        SGameObject* gameObject = nullptr;
        bool isEnable = true;

    };

}