#include "ComponentBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"
#include "../Manager/EEngineCore.h"
#include "../../../src/Object/SGameObject.h"
#include "../../../src/Component/SComponent.h"
#include "../../../src/Component/RenderComponent.h"
#include "../../../src/Component/CameraComponent.h"
#include "../../../src/Component/LightComponent.h"
#include "../../../src/Component/CustomComponent.h"

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

        response.body = "{\"status\":\"queued\"}";
        return response;
    }

    void ComponentBackend::ProcessPendingOperations() {
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
                CSE::SComponent* component = nullptr;

                if (add.componentType == "RenderComponent") {
                    component = new CSE::RenderComponent(obj);
                } else if (add.componentType == "CameraComponent") {
                    component = new CSE::CameraComponent(obj);
                } else if (add.componentType == "LightComponent") {
                    component = new CSE::LightComponent(obj);
                } else if (add.componentType == "CustomComponent") {
                    auto* customComp = new CSE::CustomComponent(obj);
                    if (!add.scriptPath.empty()) {
                        customComp->SetClassName(add.scriptPath);
                        ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                                        "Script assigned to CustomComponent",
                                        ActionParams()
                                            .Set("object", add.objectName)
                                            .Set("script", add.scriptPath));
                    }
                    component = customComp;
                }

                if (component) {
                    obj->AddComponent(component);
                    component->Init();
                    ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                                     "API: Component added",
                                     ActionParams()
                                         .Set("object", add.objectName)
                                         .Set("type", add.componentType));
                }
            }
        }
    }

}
