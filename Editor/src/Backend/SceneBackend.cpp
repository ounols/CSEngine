#include "SceneBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"
#include "../Manager/EEngineCore.h"
#include "../../../src/Object/SScene.h"
#include "../../../src/Manager/SceneMgr.h"
#include "../../../src/Util/Loader/SCENE/SSceneLoader.h"
#include "../../../src/Util/AssetsDef.h"

#include <sstream>

namespace CSEditor {

    SceneBackend& SceneBackend::GetInstance() {
        static SceneBackend instance;
        return instance;
    }

    APIResponse SceneBackend::GetSceneInfo() {
        APIResponse response;

        auto* core = EEngineCore::getEditorInstance();
        if (!core) {
            response.statusCode = 500;
            response.body = "{\"error\":\"Editor core not available\"}";
            return response;
        }

        const auto* scene = core->GetCore(SceneMgr)->GetCurrentScene();
        if (!scene) {
            response.statusCode = 404;
            response.body = "{\"error\":\"No scene loaded\"}";
            return response;
        }

        std::ostringstream oss;
        oss << "{";
        oss << "\"name\":\"" << BackendUtils::EscapeJsonString(scene->m_name) << "\"";
        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse SceneBackend::GetSceneList() {
        APIResponse response;
        response.body = "{\"scenes\":[]}";
        return response;
    }

    APIResponse SceneBackend::LoadScene(const std::string& body) {
        APIResponse response;

        std::string path = BackendUtils::ParseJsonValue(body, "path");
        bool createNew = path.empty() || path == "new";

        if (!createNew) {
            ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                             "API: Load Scene from file",
                             ActionParams().Set("path", path));
        } else {
            ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                             "API: Create new Scene",
                             ActionParams());
            path = "new";
        }

        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            m_pendingScenePath = path;
        }

        response.body = "{\"status\":\"queued\",\"path\":\"" + BackendUtils::EscapeJsonString(path) +
                       "\",\"message\":\"" + (createNew ? "New scene will be created" : "Scene will be loaded") + " on next frame\"}";
        return response;
    }

    APIResponse SceneBackend::SaveScene(const std::string& body) {
        APIResponse response;

        std::string sceneName = BackendUtils::ParseJsonValue(body, "name");
        if (sceneName.empty()) {
            sceneName = "DodgeMaster";
        }

        if (sceneName.find(".scene") == std::string::npos) {
            sceneName += ".scene";
        }

        std::string savePath = "Scene/" + sceneName;

        ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                         "API: Save Scene",
                         ActionParams().Set("path", savePath));

        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            m_pendingSceneSave = true;
            m_sceneSavePath = savePath;
        }

        response.body = "{\"status\":\"queued\",\"path\":\"" + BackendUtils::EscapeJsonString(savePath) + "\"}";
        return response;
    }

    std::string SceneBackend::ConsumePendingScenePath() {
        std::lock_guard<std::mutex> lock(m_sceneMutex);
        std::string path = m_pendingScenePath;
        m_pendingScenePath.clear();
        return path;
    }

    std::string SceneBackend::ConsumePendingSceneSavePath() {
        std::lock_guard<std::mutex> lock(m_sceneMutex);
        std::string path = m_sceneSavePath;
        m_pendingSceneSave = false;
        m_sceneSavePath.clear();
        return path;
    }

    void SceneBackend::ProcessPendingOperations() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return;

        // Process pending scene load/creation
        if (HasPendingSceneLoad()) {
            std::string scenePath = ConsumePendingScenePath();
            if (!scenePath.empty()) {
                if (scenePath == "new") {
                    auto* newScene = new CSE::SScene();
                    newScene->m_name = "New Scene";
                    core->GetCore(SceneMgr)->SetScene(newScene);
                    ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                                     "API: New scene created",
                                     ActionParams());
                } else {
                    std::string fullPath;
                    if (scenePath.find("Assets/") == 0) {
                        fullPath = CSE::NativeAssetsPath() + scenePath.substr(7);
                    } else {
                        fullPath = scenePath;
                    }

                    core->SetCurrentScene(fullPath);
                    core->ResizePreviewCore();
                    core->Update(0);
                    core->InvokeEditorRender();
                    ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                                     "API: Scene loaded from file",
                                     ActionParams().Set("path", fullPath));
                }
            }
        }

        // Process pending scene save
        if (HasPendingSceneSave()) {
            std::string savePath = ConsumePendingSceneSavePath();
            auto* scene = dynamic_cast<CSE::SScene*>(core->GetCore(SceneMgr)->GetCurrentScene());
            if (scene) {
                std::string fullPath = CSE::NativeAssetsPath() + savePath;
                bool success = CSE::SSceneLoader::SaveScene(scene, fullPath);
                if (success) {
                    ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                                     "API: Scene saved",
                                     ActionParams().Set("path", fullPath));
                } else {
                    ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::ERR,
                                     "API: Scene save failed",
                                     ActionParams().Set("path", fullPath));
                }
            }
        }
    }

    // ============================================
    // Direct Operations Implementation
    // ============================================

    bool SceneBackend::LoadSceneDirect(const std::string& path) {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return false;

        std::string fullPath;
        if (path.find("Assets/") == 0) {
            fullPath = CSE::NativeAssetsPath() + path.substr(7);
        } else {
            fullPath = path;
        }

        core->SetCurrentScene(fullPath);
        core->ResizePreviewCore();
        core->Update(0);
        core->InvokeEditorRender();

        ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                         "Scene loaded directly",
                         ActionParams().Set("path", fullPath));

        return true;
    }

    CSE::SScene* SceneBackend::CreateNewSceneDirect() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return nullptr;

        auto* newScene = new CSE::SScene();
        newScene->m_name = "New Scene";
        core->GetCore(SceneMgr)->SetScene(newScene);

        ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                         "New scene created directly",
                         ActionParams());

        return newScene;
    }

    bool SceneBackend::SaveSceneDirect(const std::string& path) {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return false;

        auto* scene = dynamic_cast<CSE::SScene*>(core->GetCore(SceneMgr)->GetCurrentScene());
        if (!scene) return false;

        std::string fullPath = path;
        if (path.find("Assets/") == std::string::npos && path.find(CSE::NativeAssetsPath()) == std::string::npos) {
            fullPath = CSE::NativeAssetsPath() + path;
        }

        bool success = CSE::SSceneLoader::SaveScene(scene, fullPath);

        if (success) {
            ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                             "Scene saved directly",
                             ActionParams().Set("path", fullPath));
        } else {
            ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::ERR,
                             "Scene save failed",
                             ActionParams().Set("path", fullPath));
        }

        return success;
    }

    std::string SceneBackend::GetCurrentSceneName() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return "";

        const auto* scene = core->GetCore(SceneMgr)->GetCurrentScene();
        if (!scene) return "";

        return scene->m_name;
    }

}
