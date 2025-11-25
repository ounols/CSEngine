#include "ComponentBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"
#include "../Manager/EEngineCore.h"
#include "../../../src/Object/SGameObject.h"
#include "../../../src/Component/SComponent.h"
#include "../../../src/Component/TransformComponent.h"
#include "../../../src/Component/RenderComponent.h"
#include "../../../src/Component/CameraComponent.h"
#include "../../../src/Component/LightComponent.h"
#include "../../../src/Component/CustomComponent.h"
#include "../../../src/Component/Animation/AnimatorComponent.h"

#include <sstream>

namespace CSEditor {

    ComponentBackend& ComponentBackend::GetInstance() {
        static ComponentBackend instance;
        return instance;
    }

    APIResponse ComponentBackend::GetComponentList(const std::string& queryParams) {
        APIResponse response;

        std::string objectName = BackendUtils::ParseQueryParam(queryParams, "object");
        if (objectName.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing object parameter\"}";
            return response;
        }

        auto* obj = BackendUtils::FindGameObjectByName(objectName);
        if (!obj) {
            response.statusCode = 404;
            response.body = "{\"error\":\"Object not found\"}";
            return response;
        }

        std::ostringstream oss;
        oss << "{\"object\":\"" << BackendUtils::EscapeJsonString(objectName) << "\",\"components\":[";

        const auto& components = obj->GetComponents();
        bool first = true;
        for (const auto& comp : components) {
            if (!first) oss << ",";
            first = false;
            oss << "{\"type\":\"" << BackendUtils::EscapeJsonString(comp->GetClassType()) << "\"}";
        }

        oss << "]}";
        response.body = oss.str();
        return response;
    }

    APIResponse ComponentBackend::AddComponent(const std::string& body) {
        APIResponse response;

        std::string objectName = BackendUtils::ParseJsonValue(body, "object");
        std::string componentType = BackendUtils::ParseJsonValue(body, "type");
        std::string scriptPath = BackendUtils::ParseJsonValue(body, "scriptPath");

        if (objectName.empty() || componentType.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing object or type parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                         "API: Add Component",
                         ActionParams()
                             .Set("object", objectName)
                             .Set("type", componentType));

        {
            std::lock_guard<std::mutex> lock(m_componentMutex);
            PendingComponentAdd add;
            add.objectName = objectName;
            add.componentType = componentType;
            add.scriptPath = scriptPath;
            m_pendingComponentAdds.push(add);
        }

        response.body = "{\"status\":\"queued\",\"object\":\"" + BackendUtils::EscapeJsonString(objectName) +
                       "\",\"type\":\"" + BackendUtils::EscapeJsonString(componentType) + "\"}";
        return response;
    }

    APIResponse ComponentBackend::RemoveComponent(const std::string& body) {
        APIResponse response;

        std::string objectName = BackendUtils::ParseJsonValue(body, "object");
        std::string componentType = BackendUtils::ParseJsonValue(body, "type");

        if (objectName.empty() || componentType.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing object or type parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                         "API: Remove Component",
                         ActionParams()
                             .Set("object", objectName)
                             .Set("type", componentType));

        {
            std::lock_guard<std::mutex> lock(m_componentMutex);
            PendingComponentRemove remove;
            remove.objectName = objectName;
            remove.componentType = componentType;
            m_pendingComponentRemoves.push(remove);
        }

        response.body = "{\"status\":\"queued\",\"object\":\"" + BackendUtils::EscapeJsonString(objectName) +
                       "\",\"type\":\"" + BackendUtils::EscapeJsonString(componentType) + "\"}";
        return response;
    }

    void ComponentBackend::ProcessPendingOperations() {
        // Process pending component adds
        while (true) {
            PendingComponentAdd add;
            {
                std::lock_guard<std::mutex> lock(m_componentMutex);
                if (m_pendingComponentAdds.empty()) break;
                add = m_pendingComponentAdds.front();
                m_pendingComponentAdds.pop();
            }

            auto* obj = BackendUtils::FindGameObjectByName(add.objectName);
            if (obj) {
                AddComponentDirect(obj, add.componentType, add.scriptPath);
            }
        }

        // Process pending component removes
        while (true) {
            PendingComponentRemove remove;
            {
                std::lock_guard<std::mutex> lock(m_componentMutex);
                if (m_pendingComponentRemoves.empty()) break;
                remove = m_pendingComponentRemoves.front();
                m_pendingComponentRemoves.pop();
            }

            auto* obj = BackendUtils::FindGameObjectByName(remove.objectName);
            if (obj) {
                const auto& components = obj->GetComponents();
                for (auto* comp : components) {
                    if (comp->GetClassType() == remove.componentType) {
                        RemoveComponentDirect(obj, comp);
                        break;
                    }
                }
            }
        }
    }

    // ============================================
    // Direct Operations Implementation
    // ============================================

    CSE::SComponent* ComponentBackend::AddComponentDirect(CSE::SGameObject* object,
                                                         const std::string& componentType,
                                                         const std::string& scriptPath) {
        if (!object) return nullptr;

        CSE::SComponent* component = nullptr;

        component = object->CreateComponent(componentType.c_str());

        if (componentType == "CustomComponent") {
            if (!scriptPath.empty()) {
                static_cast<CSE::CustomComponent*>(component)->SetClassName(scriptPath);
            }
        }

        if (component) {
            ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                             "Added component",
                             ActionParams()
                                 .Set("object", object->GetName())
                                 .Set("type", componentType));
        }

        return component;
    }

    bool ComponentBackend::RemoveComponentDirect(CSE::SGameObject* object, CSE::SComponent* component) {
        if (!object || !component) return false;

        // Don't allow removing TransformComponent
        if (component->IsSameClass("TransformComponent")) return false;

        const std::string componentType = component->GetClassType();
        const std::string objectName = object->GetName();

        object->DeleteComponent(component);

        ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                         "Removed component directly",
                         ActionParams()
                             .Set("object", objectName)
                             .Set("type", componentType));

        return true;
    }

    bool ComponentBackend::HasComponent(const CSE::SGameObject* object, const std::string& componentType) {
        if (!object) return false;

        const auto& components = object->GetComponents();
        for (const auto& comp : components) {
            if (comp->GetClassType() == componentType) {
                return true;
            }
        }
        return false;
    }

}
