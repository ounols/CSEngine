#include "ObjectBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"
#include "../Manager/EEngineCore.h"
#include "../../../src/Object/SGameObject.h"
#include "../../../src/Object/SScene.h"
#include "../../../src/Manager/SceneMgr.h"
#include "../../../src/Component/TransformComponent.h"

#include <sstream>

namespace CSEditor {

    ObjectBackend& ObjectBackend::GetInstance() {
        static ObjectBackend instance;
        return instance;
    }

    APIResponse ObjectBackend::GetObjectList() {
        APIResponse response;

        auto* core = EEngineCore::getEditorInstance();
        if (!core) {
            response.statusCode = 500;
            response.body = "{\"error\":\"Editor core not available\"}";
            return response;
        }

        auto* scene = dynamic_cast<CSE::SScene*>(core->GetCore(SceneMgr)->GetCurrentScene());
        if (!scene) {
            response.statusCode = 404;
            response.body = "{\"error\":\"No scene loaded\"}";
            return response;
        }

        std::ostringstream oss;
        oss << "{\"objects\":[";

        auto* root = scene->GetRoot();
        if (root) {
            bool first = true;
            const auto& children = root->GetChildren();
            for (const auto* child : children) {
                if (!first) oss << ",";
                first = false;
                oss << BackendUtils::GameObjectToJson(const_cast<CSE::SGameObject*>(child), false);
            }
        }

        oss << "]}";
        response.body = oss.str();
        return response;
    }

    APIResponse ObjectBackend::GetObjectInfo(const std::string& queryParams) {
        APIResponse response;

        std::string name = BackendUtils::ParseQueryParam(queryParams, "name");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing name parameter\"}";
            return response;
        }

        auto* obj = BackendUtils::FindGameObjectByName(name);
        if (!obj) {
            response.statusCode = 404;
            response.body = "{\"error\":\"Object not found\"}";
            return response;
        }

        response.body = BackendUtils::GameObjectToJson(obj, true);
        return response;
    }

    APIResponse ObjectBackend::SelectObject(const std::string& queryParams) {
        APIResponse response;

        std::string name = BackendUtils::ParseQueryParam(queryParams, "name");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing name parameter\"}";
            return response;
        }

        auto* obj = BackendUtils::FindGameObjectByName(name);
        if (!obj) {
            response.statusCode = 404;
            response.body = "{\"error\":\"Object not found\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::SELECTION, ActionSeverity::INFO,
                         "API: Select Object",
                         ActionParams().Set("name", name));

        response.body = "{\"status\":\"ok\",\"found\":\"" + BackendUtils::EscapeJsonString(name) + "\"}";
        return response;
    }

    APIResponse ObjectBackend::CreateObject(const std::string& body) {
        APIResponse response;

        std::string type = BackendUtils::ParseJsonValue(body, "type");
        std::string name = BackendUtils::ParseJsonValue(body, "name");

        if (type.empty()) {
            type = "empty";
        }
        if (name.empty()) {
            name = "New GameObject";
        }

        ACTION_LOG_PARAMS(ActionCategory::GAMEOBJECT, ActionSeverity::INFO,
                         "API: Create Object",
                         ActionParams()
                             .Set("type", type)
                             .Set("name", name));

        {
            std::lock_guard<std::mutex> lock(m_objectMutex);
            PendingObjectCreate create;
            create.type = type;
            create.name = name;
            m_pendingObjectCreates.push(create);
        }

        response.body = "{\"status\":\"queued\",\"type\":\"" + BackendUtils::EscapeJsonString(type) +
                       "\",\"name\":\"" + BackendUtils::EscapeJsonString(name) + "\"}";
        return response;
    }

    APIResponse ObjectBackend::DeleteObject(const std::string& body) {
        APIResponse response;

        std::string name = BackendUtils::ParseJsonValue(body, "name");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing name parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::GAMEOBJECT, ActionSeverity::INFO,
                         "API: Delete Object",
                         ActionParams().Set("name", name));

        response.body = "{\"status\":\"queued\",\"name\":\"" + BackendUtils::EscapeJsonString(name) + "\"}";
        return response;
    }

    APIResponse ObjectBackend::SetTransform(const std::string& body) {
        APIResponse response;

        std::string name = BackendUtils::ParseJsonValue(body, "object");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing object parameter\"}";
            return response;
        }

        PendingTransformSet transform;
        transform.objectName = name;
        transform.setPosition = false;
        transform.setRotation = false;
        transform.setScale = false;

        std::string posX = BackendUtils::ParseJsonValue(body, "posX");
        std::string posY = BackendUtils::ParseJsonValue(body, "posY");
        std::string posZ = BackendUtils::ParseJsonValue(body, "posZ");
        if (!posX.empty() && !posY.empty() && !posZ.empty()) {
            transform.posX = std::stof(posX);
            transform.posY = std::stof(posY);
            transform.posZ = std::stof(posZ);
            transform.setPosition = true;
        }

        std::string rotX = BackendUtils::ParseJsonValue(body, "rotX");
        std::string rotY = BackendUtils::ParseJsonValue(body, "rotY");
        std::string rotZ = BackendUtils::ParseJsonValue(body, "rotZ");
        std::string rotW = BackendUtils::ParseJsonValue(body, "rotW");
        if (!rotX.empty() && !rotY.empty() && !rotZ.empty() && !rotW.empty()) {
            transform.rotX = std::stof(rotX);
            transform.rotY = std::stof(rotY);
            transform.rotZ = std::stof(rotZ);
            transform.rotW = std::stof(rotW);
            transform.setRotation = true;
        }

        std::string scaleX = BackendUtils::ParseJsonValue(body, "scaleX");
        std::string scaleY = BackendUtils::ParseJsonValue(body, "scaleY");
        std::string scaleZ = BackendUtils::ParseJsonValue(body, "scaleZ");
        if (!scaleX.empty() && !scaleY.empty() && !scaleZ.empty()) {
            transform.scaleX = std::stof(scaleX);
            transform.scaleY = std::stof(scaleY);
            transform.scaleZ = std::stof(scaleZ);
            transform.setScale = true;
        }

        if (!transform.setPosition && !transform.setRotation && !transform.setScale) {
            response.statusCode = 400;
            response.body = "{\"error\":\"No transform data provided (posX/Y/Z, rotX/Y/Z/W, or scaleX/Y/Z)\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::TRANSFORM, ActionSeverity::INFO,
                         "API: Set Transform",
                         ActionParams().Set("object", name));

        {
            std::lock_guard<std::mutex> lock(m_transformMutex);
            m_pendingTransformSets.push(transform);
        }

        response.body = "{\"status\":\"queued\",\"object\":\"" + BackendUtils::EscapeJsonString(name) + "\"}";
        return response;
    }

    void ObjectBackend::ProcessPendingOperations() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return;

        // Process pending object creates
        while (true) {
            PendingObjectCreate create;
            {
                std::lock_guard<std::mutex> lock(m_objectMutex);
                if (m_pendingObjectCreates.empty()) break;
                create = m_pendingObjectCreates.front();
                m_pendingObjectCreates.pop();
            }

            auto* scene = dynamic_cast<CSE::SScene*>(core->GetCore(SceneMgr)->GetCurrentScene());
            if (scene) {
                auto* obj = new CSE::SGameObject(create.name.c_str());
                scene->GetRoot()->AddChild(obj);
                obj->Init();

                ACTION_LOG_PARAMS(ActionCategory::GAMEOBJECT, ActionSeverity::INFO,
                                 "API: Object created",
                                 ActionParams()
                                     .Set("name", create.name)
                                     .Set("type", create.type));
            }
        }

        // Process pending transform sets
        while (true) {
            PendingTransformSet transform;
            {
                std::lock_guard<std::mutex> lock(m_transformMutex);
                if (m_pendingTransformSets.empty()) break;
                transform = m_pendingTransformSets.front();
                m_pendingTransformSets.pop();
            }

            auto* obj = BackendUtils::FindGameObjectByName(transform.objectName);
            if (obj) {
                auto* transformComp = obj->GetTransform();
                if (transformComp) {
                    if (transform.setPosition) {
                        transformComp->m_position.Set(transform.posX, transform.posY, transform.posZ);
                    }
                    if (transform.setRotation) {
                        transformComp->m_rotation.Set(transform.rotX, transform.rotY, transform.rotZ, transform.rotW);
                    }
                    if (transform.setScale) {
                        transformComp->m_scale.Set(transform.scaleX, transform.scaleY, transform.scaleZ);
                    }

                    ACTION_LOG_PARAMS(ActionCategory::TRANSFORM, ActionSeverity::INFO,
                                     "API: Transform set",
                                     ActionParams().Set("object", transform.objectName));
                }
            }
        }
    }

}
