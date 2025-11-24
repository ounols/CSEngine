#include "BackendUtils.h"
#include "../Manager/EEngineCore.h"
#include "../../../src/Object/SGameObject.h"
#include "../../../src/Manager/SceneMgr.h"
#include "../../../src/Manager/GameObjectMgr.h"
#include "../../../src/Component/TransformComponent.h"

namespace CSEditor {

    std::string BackendUtils::ParseJsonValue(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t keyPos = json.find(searchKey);
        if (keyPos == std::string::npos) return "";

        size_t colonPos = json.find(':', keyPos + searchKey.length());
        if (colonPos == std::string::npos) return "";

        size_t valueStart = colonPos + 1;
        while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t')) {
            valueStart++;
        }

        if (valueStart >= json.length()) return "";

        if (json[valueStart] == '"') {
            valueStart++;
            size_t valueEnd = json.find('"', valueStart);
            if (valueEnd == std::string::npos) return "";
            return json.substr(valueStart, valueEnd - valueStart);
        }

        size_t valueEnd = json.find_first_of(",}]", valueStart);
        if (valueEnd == std::string::npos) valueEnd = json.length();
        std::string value = json.substr(valueStart, valueEnd - valueStart);

        while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
            value.pop_back();
        }

        return value;
    }

    std::string BackendUtils::ParseQueryParam(const std::string& query, const std::string& key) {
        std::string searchKey = key + "=";
        size_t keyPos = query.find(searchKey);
        if (keyPos == std::string::npos) return "";

        size_t valueStart = keyPos + searchKey.length();
        size_t valueEnd = query.find('&', valueStart);
        if (valueEnd == std::string::npos) valueEnd = query.length();

        return query.substr(valueStart, valueEnd - valueStart);
    }

    std::string BackendUtils::EscapeJsonString(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '"':  oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b";  break;
                case '\f': oss << "\\f";  break;
                case '\n': oss << "\\n";  break;
                case '\r': oss << "\\r";  break;
                case '\t': oss << "\\t";  break;
                default:   oss << c;      break;
            }
        }
        return oss.str();
    }

    std::string BackendUtils::GameObjectToJson(CSE::SGameObject* obj, bool includeChildren) {
        if (!obj) return "null";

        std::ostringstream oss;
        oss << "{";
        oss << "\"name\":\"" << EscapeJsonString(obj->GetName()) << "\",";
        oss << "\"enabled\":" << (obj->GetIsEnable() ? "true" : "false") << ",";

        auto* transform = obj->GetTransform();
        if (transform) {
            oss << "\"transform\":{";
            oss << "\"position\":[" << transform->m_position.x << "," << transform->m_position.y << "," << transform->m_position.z << "],";
            oss << "\"rotation\":[" << transform->m_rotation.x << "," << transform->m_rotation.y << "," << transform->m_rotation.z << "," << transform->m_rotation.w << "],";
            oss << "\"scale\":[" << transform->m_scale.x << "," << transform->m_scale.y << "," << transform->m_scale.z << "]";
            oss << "},";
        }

        oss << "\"components\":[";
        const auto& components = obj->GetComponents();
        bool first = true;
        for (const auto& comp : components) {
            if (!first) oss << ",";
            first = false;
            oss << "\"" << EscapeJsonString(comp->GetClassType()) << "\"";
        }
        oss << "]";

        if (includeChildren) {
            oss << ",\"children\":[";
            const auto& children = obj->GetChildren();
            first = true;
            for (const auto* child : children) {
                if (!first) oss << ",";
                first = false;
                oss << GameObjectToJson(const_cast<CSE::SGameObject*>(child), true);
            }
            oss << "]";
        }

        oss << "}";
        return oss.str();
    }

    CSE::SGameObject* BackendUtils::FindGameObjectByName(const std::string& name) {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return nullptr;

        auto* scene = core->GetCore(SceneMgr)->GetCurrentScene();
        if (!scene) return nullptr;

        return core->GetCore(GameObjectMgr)->Find(name);
    }

}
