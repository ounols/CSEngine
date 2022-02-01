#include "CustomComponent.h"

#include <utility>
#include "../Manager/ScriptMgr.h"
#include "../MacroDef.h"
#include "../Object/SScriptObject.h"
#include "../Util/MoreComponentFunc.h"
#include "../Util/SafeLog.h"

using namespace CSE;


COMPONENT_CONSTRUCTOR(CustomComponent) {
}


CustomComponent::~CustomComponent() = default;


void CustomComponent::Exterminate() {
    SAFE_DELETE(m_specialization);
    SAFE_DELETE(m_classInstance);
}


void CustomComponent::Init() {

    if (m_specialization == nullptr) return;
    if (m_classInstance == nullptr) {
        CreateClassInstance(std::vector<std::string>());
    }


    if (m_funcInit < 0 || m_isError) return;

    try {
        m_classInstance->call(m_funcInit);
    } catch (Sqrat::Exception e) {
        m_isError = true;
		SafeLog::Log((Sqrat::LastErrorString(Sqrat::DefaultVM::Get()) + '\n').c_str());
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
		SafeLog::Log((Sqrat::LastErrorString(Sqrat::DefaultVM::Get()) + '\n').c_str());
    }

}


void CustomComponent::RegisterScript() {

    if (m_className.empty()) return;
    if (m_specialization != nullptr) SAFE_DELETE(m_specialization);

    try {
        m_specialization = new sqext::SQIClass(m_className.c_str());
        m_specialization->bind(m_funcSetCSEngine, "SetCSEngine");
    }
    catch (Sqrat::Exception e) {
        m_funcSetCSEngine = -1;
		SafeLog::Log((Sqrat::LastErrorString(Sqrat::DefaultVM::Get()) + '\n').c_str());
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
    auto asset = SResource::Get<SScriptObject>(std::move(name));
    if (asset == nullptr) return;

    m_classID = asset->GetID();
    m_className = asset->GetScriptClassName();

    RegisterScript();
    CreateClassInstance(asset->GetVariables());
//    Init();
}


std::string CustomComponent::SGetClassName() const {
    return m_className;
}


bool CustomComponent::GetIsEnable() const {
    return isEnable;
}


void CustomComponent::SetIsEnable(bool is_enable) {
    isEnable = is_enable;
}


void CustomComponent::Log(const char* log) {
	SafeLog::Log((std::string(log) + '\n').c_str());
}


SGameObject* CustomComponent::GetGameObject() const {
    return gameObject;
}

SComponent* CustomComponent::Clone(SGameObject* object) {
    INIT_COMPONENT_CLONE(CustomComponent, clone);

    if (!m_className.empty()) {
        clone->SetClassName(m_classID);
    }

    return clone;
}

void CustomComponent::SetValue(std::string name_str, VariableBinder::Arguments value) {
    if (name_str == "m_classID") {
        SetClassName(ConvertSpaceStr(value[0], true));
    }
    //variable : 0.name , 1.value, 2. type
    else if (name_str == "__variable__") {
        for(auto& val : m_variables) {
            if(val.name == value[0]) {
				val.value = value[1];
				val.type = value[2];
                BindValue(&val, ConvertSpaceStr(value[1], true).c_str());
            }
        }
    }
}

std::string CustomComponent::PrintValue() const {
    PRINT_START("component");

    PRINT_VALUE(m_classID, ConvertSpaceStr(m_classID));
    for(auto& val : m_variables) {
        std::string value = val.value;
        if(value.empty()) value = "null";
        PRINT_VALUE(__variable__, val.name, ' ', ConvertSpaceStr(value) , ' ', val.type);
    }


    PRINT_END("component");
}

void CustomComponent::CreateClassInstance(const std::vector<std::string>& variables) {
    if (m_specialization == nullptr) return;
    if (m_classInstance != nullptr) {
        SAFE_DELETE(m_classInstance);
    }

    try {
        m_classInstance = m_specialization->NewPointer();

        if (m_funcSetCSEngine < 0) return;
        m_classInstance->call(m_funcSetCSEngine, this);
    } catch (Sqrat::Exception e) {
        m_isError = true;
		SafeLog::Log((Sqrat::LastErrorString(Sqrat::DefaultVM::Get()) + '\n').c_str());
        return;
    }

    if(variables.empty()) return;

    for (const auto& val : variables) {
        auto obj = m_classInstance->get(val.c_str());
        auto r_obj = obj.GetObject()._type;

        const char* name;
        std::string value;

        switch (r_obj) {
            case OT_STRING:
                name = "str";
                value = obj.Cast<const SQChar*>();
                break;
            case OT_BOOL:
                name = "bool";
                value = obj.Cast<SQBool>() ? "t" : "f";
                break;
            case OT_FLOAT:
                name = "float";
                value = std::to_string(obj.Cast<SQFloat>());
                break;
            case OT_INTEGER:
                name = "int";
                value = std::to_string(obj.Cast<SQInteger>());
                break;
            case OT_ARRAY:
                name = "arr";
                break;
            default:
                name = "null";
                break;
        }

        m_variables.emplace_back(val, name, value);
    }

}

void CustomComponent::BindValue(CustomComponent::VARIABLE* variable, const char* value) const {

    std::string type = variable->type;
    std::string value_str = value;

    if(type == "str") m_classInstance->set(variable->name.c_str(), value);
    else if(type == "bool") m_classInstance->set(variable->name.c_str(), value_str == "t");
    else if(type == "float") m_classInstance->set(variable->name.c_str(), std::stof(value_str));
    else if(type == "int") m_classInstance->set(variable->name.c_str(), std::stoi(value_str));
//    else if(type == "arr") m_classInstance->set(variable->name.c_str(), std::stof(value_str));
    else if(type == "comp") {
        SComponent* comp = gameObject->GetSComponentByID(value_str);
        MoreComponentFunc::BindComponentToSQInstance(comp, variable->name, m_classInstance);
    }
    else if(type == "gobj") {
        SGameObject* obj = SGameObject::FindByID(value_str);
        m_classInstance->set(variable->name.c_str(), obj);
    }
    else if(type == "nut") {
        SComponent* comp = gameObject->GetSComponentByID(value_str);

        if(dynamic_cast<CustomComponent*>(comp) != nullptr) {
            auto comp_r = static_cast<CustomComponent*>(comp);
            m_classInstance->set(variable->name.c_str(), comp_r->GetClassInstance());
        }
    }

}