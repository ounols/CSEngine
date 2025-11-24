#include "EditorAPIServer.h"
#include "EditorActionLogger.h"
#include "EEngineCore.h"
#include "DebugStackTrace.h"
#include "../Objects/Base/HierarchyData.h"

// Include httplib (header-only HTTP library)
#include "../Util/httplib.h"

// Engine includes
#include "../../../src/Object/SGameObject.h"
#include "../../../src/Object/SScene.h"
#include "../../../src/Object/SScriptObject.h"
#include "../../../src/Manager/SceneMgr.h"
#include "../../../src/Manager/GameObjectMgr.h"
#include "../../../src/Component/SComponent.h"
#include "../../../src/Component/TransformComponent.h"
#include "../../../src/Component/RenderComponent.h"
#include "../../../src/Component/CameraComponent.h"
#include "../../../src/Component/LightComponent.h"
#include "../../../src/Component/CustomComponent.h"
#include "../../../src/Util/Loader/SCENE/SSceneLoader.h"
#include "../../../src/Util/AssetsDef.h"
#include "../../../src/Util/CaptureDef.h"

#include <sstream>
#include "sqrat.h"
#include <iomanip>
#include <chrono>

namespace CSEditor {

    EditorAPIServer& EditorAPIServer::GetInstance() {
        static EditorAPIServer instance;
        return instance;
    }

    EditorAPIServer::EditorAPIServer() = default;

    EditorAPIServer::~EditorAPIServer() {
        Stop();
    }

    bool EditorAPIServer::Start(int port) {
        if (m_running) {
            ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::WARNING,
                      "API Server already running", "port=" + std::to_string(m_port));
            return true;
        }

        m_port = port;
        m_shouldStop = false;

        // Start server thread
        m_serverThread = std::thread(&EditorAPIServer::ServerThread, this);

        // Wait briefly for server to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (m_running) {
            ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::INFO,
                      "API Server started", "port=" + std::to_string(m_port));
        }

        return m_running;
    }

    void EditorAPIServer::Stop() {
        if (!m_running && !m_serverThread.joinable()) {
            return;
        }

        m_shouldStop = true;

        // Wait for thread to finish
        if (m_serverThread.joinable()) {
            m_serverThread.join();
        }

        m_running = false;

        ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::INFO,
                  "API Server stopped", "");
    }

    // Helper to build error response with crash history
    APIResponse EditorAPIServer::BuildErrorResponseWithDebugInfo(const std::string& error, const std::string& endpoint) {
        APIResponse response;
        response.statusCode = 500;

        std::ostringstream oss;
        oss << "{";
        oss << "\"error\":\"" << EscapeJsonString(error) << "\",";
        oss << "\"endpoint\":\"" << EscapeJsonString(endpoint) << "\",";

        // Include recent action history for debugging
        auto recentActions = DebugStackTrace::GetRecentActions(20);
        oss << "\"recentActions\":[";
        bool first = true;
        for (const auto& entry : recentActions) {
            if (!first) oss << ",";
            first = false;
            oss << "{";
            oss << "\"action\":\"" << EscapeJsonString(entry.action) << "\",";
            oss << "\"context\":\"" << EscapeJsonString(entry.context) << "\",";
            oss << "\"timestamp\":\"" << EscapeJsonString(entry.timestamp) << "\"";
            oss << "}";
        }
        oss << "],";

        // Include system info
        oss << "\"systemInfo\":" << DebugStackTrace::GetSystemInfo().ToJson() << ",";

        // Capture stack trace
        auto stackFrames = DebugStackTrace::Capture(2, 20);
        oss << "\"stackTrace\":[";
        first = true;
        for (const auto& frame : stackFrames) {
            if (!first) oss << ",";
            first = false;
            oss << "{";
            oss << "\"function\":\"" << EscapeJsonString(frame.function) << "\",";
            oss << "\"file\":\"" << EscapeJsonString(frame.file) << "\",";
            oss << "\"line\":" << frame.line;
            oss << "}";
        }
        oss << "]";

        oss << "}";

        response.body = oss.str();

        // Log the error
        ACTION_LOG_PARAMS(ActionCategory::SYSTEM, ActionSeverity::ERR,
                         "API Error",
                         ActionParams()
                             .Set("endpoint", endpoint)
                             .Set("error", error));

        return response;
    }

    void EditorAPIServer::ServerThread() {
        httplib::Server svr;

        // CORS headers for browser access
        auto setCorsHeaders = [](httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
        };

        // Safe handler wrapper that catches exceptions and returns debug info
        auto safeHandler = [this](const std::string& endpoint, std::function<APIResponse()> handler) -> APIResponse {
            try {
                return handler();
            } catch (const std::exception& e) {
                m_errorCount++;
                return BuildErrorResponseWithDebugInfo(std::string("Exception: ") + e.what(), endpoint);
            } catch (...) {
                m_errorCount++;
                return BuildErrorResponseWithDebugInfo("Unknown exception occurred", endpoint);
            }
        };

        // OPTIONS handler for CORS preflight
        svr.Options(".*", [&setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            res.status = 204;
        });

        // Health check endpoint
        svr.Get("/api/health", [this, &setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            res.set_content("{\"status\":\"ok\",\"server\":\"CSEditor API\"}", "application/json");
        });

        // Scene endpoints
        svr.Get("/api/scene/info", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/scene/info", [this]() { return HandleSceneInfo(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/scene/list", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/scene/list", [this]() { return HandleSceneList(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/scene/load", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/scene/load", [this, body]() { return HandleSceneLoad(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/scene/save", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/scene/save", [this]() { return HandleSceneSave(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Object endpoints
        svr.Get("/api/object/list", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/object/list", [this]() { return HandleObjectList(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/object/info", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/object/info", [this, query]() { return HandleObjectInfo(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/object/select", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/object/select", [this, query]() { return HandleObjectSelect(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/object/create", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/object/create", [this, body]() { return HandleObjectCreate(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/object/delete", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/object/delete", [this, body]() { return HandleObjectDelete(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/object/set-transform", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/object/set-transform", [this, body]() { return HandleTransformSet(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Component endpoints
        svr.Get("/api/component/list", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/component/list", [this, query]() { return HandleComponentList(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/component/add", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/component/add", [this, body]() { return HandleComponentAdd(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/component/remove", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/component/remove", [this, body]() { return HandleComponentRemove(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Log endpoints
        svr.Get("/api/log/recent", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/log/recent", [this, query]() { return HandleLogRecent(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/log/clear", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/log/clear", [this]() { return HandleLogClear(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Editor endpoints
        svr.Get("/api/editor/info", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/editor/info", [this]() { return HandleEditorInfo(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/editor/command", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/editor/command", [this, body]() { return HandleEditorCommand(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/editor/capture-preview", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/editor/capture-preview", [this, query]() { return HandleCapturePreview(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Debug/Crash endpoints (these don't use safeHandler to avoid infinite recursion on errors)
        svr.Get("/api/debug/crash", [this, &setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = HandleDebugCrashInfo();
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/debug/context", [this, &setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = HandleDebugContext();
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/debug/history", [this, &setCorsHeaders](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = HandleDebugHistory(query);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/debug/dump", [this, &setCorsHeaders](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = HandleDebugTriggerDump(req.body);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Server status
        svr.Get("/api/status", [this, &setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            res.set_content(GetStatusJson(), "application/json");
        });

        // Set up a stop check thread
        std::thread stopChecker([this, &svr]() {
            while (!m_shouldStop) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            svr.stop();
        });

        m_running = true;

        // Run server (blocking until svr.stop() is called)
        if (!svr.listen("0.0.0.0", m_port)) {
            m_running = false;
            m_errorCount++;
            ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::ERR,
                      "API Server failed to start", "port=" + std::to_string(m_port));
        }

        // Signal stop and wait for checker thread
        m_shouldStop = true;
        if (stopChecker.joinable()) {
            stopChecker.join();
        }

        m_running = false;
    }

    void EditorAPIServer::ProcessPendingRequests() {
        std::lock_guard<std::mutex> lock(m_requestMutex);

        while (!m_pendingRequests.empty()) {
            auto& pair = m_pendingRequests.front();
            auto response = HandleRequest(pair.first);
            pair.second(response);
            m_pendingRequests.pop();
        }
    }

    APIResponse EditorAPIServer::HandleRequest(const APIRequest& request) {
        m_requestCount++;

        ACTION_LOG_PARAMS(ActionCategory::SYSTEM, ActionSeverity::DEBUG,
                         "API Request",
                         ActionParams()
                             .Set("endpoint", request.endpoint)
                             .Set("method", request.method)
                             .Set("requestId", static_cast<int>(request.requestId)));

        // Route to appropriate handler
        // This is a fallback - most routing is done in ServerThread
        APIResponse response;
        response.statusCode = 404;
        response.body = "{\"error\":\"Unknown endpoint\"}";
        return response;
    }

    std::string EditorAPIServer::GetStatusJson() const {
        std::ostringstream oss;
        oss << "{";
        oss << "\"running\":" << (m_running ? "true" : "false") << ",";
        oss << "\"port\":" << m_port << ",";
        oss << "\"requestCount\":" << m_requestCount << ",";
        oss << "\"errorCount\":" << m_errorCount;
        oss << "}";
        return oss.str();
    }

    // Scene handlers
    APIResponse EditorAPIServer::HandleSceneInfo() {
        m_requestCount++;
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
        oss << "\"name\":\"" << EscapeJsonString(scene->m_name) << "\"";
        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleSceneList() {
        m_requestCount++;
        APIResponse response;

        // Return list of available scenes
        // For now, return current scene info
        response.body = "{\"scenes\":[]}";
        return response;
    }

    APIResponse EditorAPIServer::HandleSceneLoad(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string path = ParseJsonValue(body, "path");
        
        // If path is empty or "new", create a new scene instead of loading from file
        bool createNew = path.empty() || path == "new";
        
        if (!createNew) {
            ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                             "API: Load Scene from file",
                             ActionParams().Set("path", path));
        } else {
            ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                             "API: Create new Scene",
                             ActionParams());
            path = "new";  // Mark as new scene creation
        }

        // Queue scene load/creation for main thread processing
        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            m_pendingScenePath = path;
        }

        response.body = "{\"status\":\"queued\",\"path\":\"" + EscapeJsonString(path) + 
                       "\",\"message\":\"" + (createNew ? "New scene will be created" : "Scene will be loaded") + " on next frame\"}";
        return response;
    }

    std::string EditorAPIServer::ConsumePendingScenePath() {
        std::lock_guard<std::mutex> lock(m_sceneMutex);
        std::string path = m_pendingScenePath;
        m_pendingScenePath.clear();
        return path;
    }

    void EditorAPIServer::ProcessMainThreadCommands() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return;

        // Process pending scene load/creation
        if (HasPendingSceneLoad()) {
            std::string scenePath = ConsumePendingScenePath();
            if (!scenePath.empty()) {
                if (scenePath == "new") {
                    // Create a new empty scene instead of loading from file
                    auto* newScene = new CSE::SScene();
                    newScene->m_name = "New Scene";
                    core->GetCore(SceneMgr)->SetScene(newScene);
                    ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                                     "API: New scene created",
                                     ActionParams());
                } else {
                    // Load scene from file (following AssetWindow::OnAssetClickEvent logic)
                    // Convert relative path to absolute path
                    std::string fullPath;
                    if (scenePath.find("Assets/") == 0) {
                        // Path starts with "Assets/" - convert to absolute path
                        fullPath = CSE::NativeAssetsPath() + scenePath.substr(7); // Remove "Assets/" prefix
                    } else {
                        // Assume it's already a full path or AssetMgr path
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
        bool shouldSave = false;
        std::string savePath;
        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            if (m_pendingSceneSave) {
                shouldSave = true;
                savePath = m_sceneSavePath;
                m_pendingSceneSave = false;
                m_sceneSavePath.clear();
            }
        }
        
        if (shouldSave) {
            auto* scene = dynamic_cast<CSE::SScene*>(core->GetCore(SceneMgr)->GetCurrentScene());
            if (scene) {
                // Convert to absolute path using NativeAssetsPath
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
                // Note: SGameObject constructor already creates TransformComponent and registers to GameObjectMgr
                scene->GetRoot()->AddChild(obj);
                obj->Init();  // Initialize the object after adding to scene
                
                ACTION_LOG_PARAMS(ActionCategory::GAMEOBJECT, ActionSeverity::INFO,
                                 "API: Object created",
                                 ActionParams()
                                     .Set("name", create.name)
                                     .Set("type", create.type));
            }
        }

        // Process pending component adds
        while (true) {
            PendingComponentAdd add;
            {
                std::lock_guard<std::mutex> lock(m_componentMutex);
                if (m_pendingComponentAdds.empty()) break;
                add = m_pendingComponentAdds.front();
                m_pendingComponentAdds.pop();
            }

            auto* obj = FindGameObjectByName(add.objectName);
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
                        // SetClassName will use the already-initialized ScriptMgr
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
                    component->Init();  // Initialize component after adding
                    ACTION_LOG_PARAMS(ActionCategory::COMPONENT, ActionSeverity::INFO,
                                     "API: Component added",
                                     ActionParams()
                                         .Set("object", add.objectName)
                                         .Set("type", add.componentType));
                }
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

            auto* obj = FindGameObjectByName(transform.objectName);
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

        // Process pending preview captures
        while (true) {
            PendingPreviewCapture capture;
            {
                std::lock_guard<std::mutex> lock(m_previewCaptureMutex);
                if (m_pendingPreviewCaptures.empty()) break;
                capture = m_pendingPreviewCaptures.front();
                m_pendingPreviewCaptures.pop();
            }

            // Execute the capture on the main thread (where OpenGL context is available)
            unsigned int previewTextureId = core->GetPreviewTextureId();
            if (previewTextureId == 0) {
                *capture.success = false;
                continue;
            }

            // Bind texture to get dimensions
            glBindTexture(GL_TEXTURE_2D, previewTextureId);
            GLint width, height;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (width <= 0 || height <= 0) {
                *capture.success = false;
                continue;
            }

            // Create temporary FBO for capture (following SEnvironmentMgr pattern)
            GLuint captureFBO;
            glGenFramebuffers(1, &captureFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            
            // Attach the preview texture to the FBO (glFramebufferTexture2D as required)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                                   GL_TEXTURE_2D, previewTextureId, 0);

            // Check FBO status
            GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
                glDeleteFramebuffers(1, &captureFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                *capture.success = false;
                continue;
            }

            // Read pixels from the framebuffer
            char* data = new char[width * height * 4];
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

            // Clean up FBO
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &captureFBO);

            // Save as PNG
            std::string fullPath = CSE::NativeAssetsPath() + "../" + capture.filename;
            int saved = CSE::savePng(fullPath.c_str(), width, height, 4, data);
            delete[] data;

            // Set output parameters
            *capture.resultPath = fullPath;
            *capture.resultWidth = width;
            *capture.resultHeight = height;
            *capture.success = (saved != 0);
        }
    }

    APIResponse EditorAPIServer::HandleSceneSave() {
        m_requestCount++;
        APIResponse response;

        // Save to Scene folder (NativeAssetsPath already includes "Assets/")
        std::string savePath = "Scene/DodgeMaster.scene";

        ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                         "API: Save Scene",
                         ActionParams().Set("path", savePath));

        // Queue scene save for main thread processing
        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            m_pendingSceneSave = true;
            m_sceneSavePath = savePath;
        }

        response.body = "{\"status\":\"queued\",\"path\":\"" + EscapeJsonString(savePath) + "\"}";
        return response;
    }

    // Object handlers
    APIResponse EditorAPIServer::HandleObjectList() {
        m_requestCount++;
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
                oss << GameObjectToJson(const_cast<CSE::SGameObject*>(child), false);
            }
        }

        oss << "]}";
        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleObjectInfo(const std::string& queryParams) {
        m_requestCount++;
        APIResponse response;

        std::string name = ParseQueryParam(queryParams, "name");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing name parameter\"}";
            return response;
        }

        auto* obj = FindGameObjectByName(name);
        if (!obj) {
            response.statusCode = 404;
            response.body = "{\"error\":\"Object not found\"}";
            return response;
        }

        response.body = GameObjectToJson(obj, true);
        return response;
    }

    APIResponse EditorAPIServer::HandleObjectSelect(const std::string& queryParams) {
        m_requestCount++;
        APIResponse response;

        std::string name = ParseQueryParam(queryParams, "name");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing name parameter\"}";
            return response;
        }

        auto* obj = FindGameObjectByName(name);
        if (!obj) {
            response.statusCode = 404;
            response.body = "{\"error\":\"Object not found\"}";
            return response;
        }

        // Log the selection request - actual selection will be handled on main thread
        ACTION_LOG_PARAMS(ActionCategory::SELECTION, ActionSeverity::INFO,
                         "API: Select Object",
                         ActionParams().Set("name", name));

        // Note: Selection needs to be queued for main thread processing
        // For now, just return success if object exists
        response.body = "{\"status\":\"ok\",\"found\":\"" + EscapeJsonString(name) + "\"}";

        return response;
    }

    APIResponse EditorAPIServer::HandleObjectCreate(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string type = ParseJsonValue(body, "type");
        std::string name = ParseJsonValue(body, "name");

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

        // Queue object creation for main thread processing
        {
            std::lock_guard<std::mutex> lock(m_objectMutex);
            PendingObjectCreate create;
            create.type = type;
            create.name = name;
            m_pendingObjectCreates.push(create);
        }

        response.body = "{\"status\":\"queued\",\"type\":\"" + EscapeJsonString(type) +
                       "\",\"name\":\"" + EscapeJsonString(name) + "\"}";
        return response;
    }

    APIResponse EditorAPIServer::HandleObjectDelete(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string name = ParseJsonValue(body, "name");
        if (name.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing name parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::GAMEOBJECT, ActionSeverity::INFO,
                         "API: Delete Object",
                         ActionParams().Set("name", name));

        response.body = "{\"status\":\"queued\",\"name\":\"" + EscapeJsonString(name) + "\"}";
        return response;
    }

    APIResponse EditorAPIServer::HandleTransformSet(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string name = ParseJsonValue(body, "object");
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

        // Parse position if present
        std::string posX = ParseJsonValue(body, "posX");
        std::string posY = ParseJsonValue(body, "posY");
        std::string posZ = ParseJsonValue(body, "posZ");
        if (!posX.empty() && !posY.empty() && !posZ.empty()) {
            transform.posX = std::stof(posX);
            transform.posY = std::stof(posY);
            transform.posZ = std::stof(posZ);
            transform.setPosition = true;
        }

        // Parse rotation if present
        std::string rotX = ParseJsonValue(body, "rotX");
        std::string rotY = ParseJsonValue(body, "rotY");
        std::string rotZ = ParseJsonValue(body, "rotZ");
        std::string rotW = ParseJsonValue(body, "rotW");
        if (!rotX.empty() && !rotY.empty() && !rotZ.empty() && !rotW.empty()) {
            transform.rotX = std::stof(rotX);
            transform.rotY = std::stof(rotY);
            transform.rotZ = std::stof(rotZ);
            transform.rotW = std::stof(rotW);
            transform.setRotation = true;
        }

        // Parse scale if present
        std::string scaleX = ParseJsonValue(body, "scaleX");
        std::string scaleY = ParseJsonValue(body, "scaleY");
        std::string scaleZ = ParseJsonValue(body, "scaleZ");
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

        // Queue transform set for main thread processing
        {
            std::lock_guard<std::mutex> lock(m_transformMutex);
            m_pendingTransformSets.push(transform);
        }

        response.body = "{\"status\":\"queued\",\"object\":\"" + EscapeJsonString(name) + "\"}";
        return response;
    }

    // Component handlers
    APIResponse EditorAPIServer::HandleComponentList(const std::string& queryParams) {
        m_requestCount++;
        APIResponse response;

        std::string objectName = ParseQueryParam(queryParams, "object");
        if (objectName.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing object parameter\"}";
            return response;
        }

        auto* obj = FindGameObjectByName(objectName);
        if (!obj) {
            response.statusCode = 404;
            response.body = "{\"error\":\"Object not found\"}";
            return response;
        }

        std::ostringstream oss;
        oss << "{\"object\":\"" << EscapeJsonString(objectName) << "\",\"components\":[";

        const auto& components = obj->GetComponents();
        bool first = true;
        for (const auto& comp : components) {
            if (!first) oss << ",";
            first = false;
            oss << "{\"type\":\"" << EscapeJsonString(comp->GetClassType()) << "\"}";
        }

        oss << "]}";
        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleComponentAdd(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string objectName = ParseJsonValue(body, "object");
        std::string componentType = ParseJsonValue(body, "type");
        std::string scriptPath = ParseJsonValue(body, "scriptPath");  // Optional, for CustomComponent (e.g., "PlayerController")

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

        // Queue component addition for main thread processing
        {
            std::lock_guard<std::mutex> lock(m_componentMutex);
            PendingComponentAdd add;
            add.objectName = objectName;
            add.componentType = componentType;
            add.scriptPath = scriptPath;  // For CustomComponent, this is the class name (not full path)
            m_pendingComponentAdds.push(add);
        }

        response.body = "{\"status\":\"queued\",\"object\":\"" + EscapeJsonString(objectName) +
                       "\",\"type\":\"" + EscapeJsonString(componentType) + "\"}";
        return response;
    }

    APIResponse EditorAPIServer::HandleComponentRemove(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string objectName = ParseJsonValue(body, "object");
        std::string componentType = ParseJsonValue(body, "type");

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

    // Log handlers
    APIResponse EditorAPIServer::HandleLogRecent(const std::string& queryParams) {
        m_requestCount++;
        APIResponse response;

        std::string countStr = ParseQueryParam(queryParams, "count");
        size_t count = countStr.empty() ? 20 : std::stoul(countStr);

        auto& logger = EditorActionLogger::GetInstance();
        response.body = logger.GetEntriesAsJson(count);
        return response;
    }

    APIResponse EditorAPIServer::HandleLogClear() {
        m_requestCount++;
        APIResponse response;

        EditorActionLogger::GetInstance().ClearLogs();

        ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::INFO, "API: Logs cleared", "");

        response.body = "{\"status\":\"ok\"}";
        return response;
    }

    // Editor handlers
    APIResponse EditorAPIServer::HandleEditorInfo() {
        m_requestCount++;
        APIResponse response;

        auto* core = EEngineCore::getEditorInstance();
        std::ostringstream oss;
        oss << "{";
        oss << "\"version\":\"CSEditor 1.0\",";
        oss << "\"isPreview\":" << (core && core->IsPreview() ? "true" : "false") << ",";
        oss << "\"apiPort\":" << m_port << ",";
        oss << "\"sessionStart\":\"" << EditorActionLogger::GetInstance().GetSessionStartTime() << "\"";
        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleEditorCommand(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string command = ParseJsonValue(body, "command");
        if (command.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing command parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::EDITOR, ActionSeverity::INFO,
                         "API: Editor Command",
                         ActionParams().Set("command", command));

        // Handle known commands
        if (command == "play") {
            auto* core = EEngineCore::getEditorInstance();
            if (core && !core->IsPreview()) {
                // Start with default resolution (can be resized later)
                core->InvokePreviewStart(1280, 720);
                response.body = "{\"status\":\"ok\",\"command\":\"play\",\"resolution\":\"1280x720\"}";
            } else {
                response.body = "{\"status\":\"already_playing\"}";
            }
        } else if (command == "stop") {
            auto* core = EEngineCore::getEditorInstance();
            if (core && core->IsPreview()) {
                core->InvokePreviewStop();
                response.body = "{\"status\":\"ok\",\"command\":\"stop\"}";
            } else {
                response.body = "{\"status\":\"not_playing\"}";
            }
        } else {
            response.body = "{\"status\":\"queued\",\"command\":\"" + EscapeJsonString(command) + "\"}";
        }

        return response;
    }

    APIResponse EditorAPIServer::HandleCapturePreview(const std::string& queryParams) {
        m_requestCount++;
        APIResponse response;

        auto* core = EEngineCore::getEditorInstance();
        if (!core) {
            response.statusCode = 500;
            response.body = "{\"error\":\"Editor core not available\"}";
            return response;
        }

        // Get filename from query params (optional)
        std::string filename = ParseQueryParam(queryParams, "filename");
        if (filename.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif
            std::ostringstream oss;
            oss << "preview_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".png";
            filename = oss.str();
        }

        // Ensure .png extension
        if (filename.find(".png") == std::string::npos) {
            filename += ".png";
        }

        // Queue the capture request to be processed on the main thread
        std::string resultPath;
        int resultWidth = 0, resultHeight = 0;
        bool success = false;
        
        {
            std::lock_guard<std::mutex> lock(m_previewCaptureMutex);
            m_pendingPreviewCaptures.push({
                filename,
                &resultPath,
                &resultWidth,
                &resultHeight,
                &success
            });
        }

        // Wait for the capture to be processed (with timeout)
        int waitCount = 0;
        while (waitCount < 100 && !success) {  // Wait up to 1 second (100 * 10ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            waitCount++;
        }

        if (success) {
            ACTION_LOG_PARAMS(ActionCategory::SYSTEM, ActionSeverity::INFO,
                            "Preview captured",
                            ActionParams()
                                .Set("filename", filename)
                                .Set("width", std::to_string(resultWidth))
                                .Set("height", std::to_string(resultHeight)));

            std::ostringstream jsonResponse;
            jsonResponse << "{";
            jsonResponse << "\"success\":true,";
            jsonResponse << "\"filename\":\"" << filename << "\",";
            jsonResponse << "\"path\":\"" << resultPath << "\",";
            jsonResponse << "\"width\":" << resultWidth << ",";
            jsonResponse << "\"height\":" << resultHeight;
            jsonResponse << "}";

            response.statusCode = 200;
            response.body = jsonResponse.str();
        } else {
            response.statusCode = 500;
            response.body = "{\"error\":\"Failed to capture preview (timeout or error)\"}";
        }

        return response;
    }

    // Utility functions
    std::string EditorAPIServer::ParseJsonValue(const std::string& json, const std::string& key) const {
        // Simple JSON parser - looks for "key":"value" pattern
        std::string searchKey = "\"" + key + "\"";
        size_t keyPos = json.find(searchKey);
        if (keyPos == std::string::npos) return "";

        size_t colonPos = json.find(':', keyPos + searchKey.length());
        if (colonPos == std::string::npos) return "";

        // Skip whitespace
        size_t valueStart = colonPos + 1;
        while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t')) {
            valueStart++;
        }

        if (valueStart >= json.length()) return "";

        // Check if value is string
        if (json[valueStart] == '"') {
            valueStart++;
            size_t valueEnd = json.find('"', valueStart);
            if (valueEnd == std::string::npos) return "";
            return json.substr(valueStart, valueEnd - valueStart);
        }

        // Non-string value (number, bool, null)
        size_t valueEnd = json.find_first_of(",}]", valueStart);
        if (valueEnd == std::string::npos) valueEnd = json.length();
        std::string value = json.substr(valueStart, valueEnd - valueStart);

        // Trim whitespace
        while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
            value.pop_back();
        }

        return value;
    }

    std::string EditorAPIServer::ParseQueryParam(const std::string& query, const std::string& key) const {
        std::string searchKey = key + "=";
        size_t keyPos = query.find(searchKey);
        if (keyPos == std::string::npos) return "";

        size_t valueStart = keyPos + searchKey.length();
        size_t valueEnd = query.find('&', valueStart);
        if (valueEnd == std::string::npos) valueEnd = query.length();

        return query.substr(valueStart, valueEnd - valueStart);
    }

    std::string EditorAPIServer::EscapeJsonString(const std::string& str) const {
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

    std::string EditorAPIServer::GameObjectToJson(CSE::SGameObject* obj, bool includeChildren) const {
        if (!obj) return "null";

        std::ostringstream oss;
        oss << "{";
        oss << "\"name\":\"" << EscapeJsonString(obj->GetName()) << "\",";
        oss << "\"enabled\":" << (obj->GetIsEnable() ? "true" : "false") << ",";

        // Transform
        auto* transform = obj->GetTransform();
        if (transform) {
            oss << "\"transform\":{";
            oss << "\"position\":[" << transform->m_position.x << "," << transform->m_position.y << "," << transform->m_position.z << "],";
            oss << "\"rotation\":[" << transform->m_rotation.x << "," << transform->m_rotation.y << "," << transform->m_rotation.z << "," << transform->m_rotation.w << "],";
            oss << "\"scale\":[" << transform->m_scale.x << "," << transform->m_scale.y << "," << transform->m_scale.z << "]";
            oss << "},";
        }

        // Components
        oss << "\"components\":[";
        const auto& components = obj->GetComponents();
        bool first = true;
        for (const auto& comp : components) {
            if (!first) oss << ",";
            first = false;
            oss << "\"" << EscapeJsonString(comp->GetClassType()) << "\"";
        }
        oss << "]";

        // Children
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

    CSE::SGameObject* EditorAPIServer::FindGameObjectByName(const std::string& name) const {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return nullptr;

        auto* scene = core->GetCore(SceneMgr)->GetCurrentScene();
        if (!scene) return nullptr;

        return core->GetCore(GameObjectMgr)->Find(name);
    }

    // Debug/Crash handlers
    APIResponse EditorAPIServer::HandleDebugCrashInfo() {
        m_requestCount++;
        APIResponse response;

        // Get recent actions which include crash info if any
        auto recentActions = DebugStackTrace::GetRecentActions(50);

        std::ostringstream oss;
        oss << "{\"crashHistory\":[";

        bool first = true;
        for (const auto& entry : recentActions) {
            // Filter for crash-related actions
            if (entry.action.find("CRASH") != std::string::npos ||
                entry.action.find("ERROR") != std::string::npos ||
                entry.action.find("Exception") != std::string::npos ||
                entry.action.find("SIGSEGV") != std::string::npos ||
                entry.action.find("ACCESS_VIOLATION") != std::string::npos) {

                if (!first) oss << ",";
                first = false;

                oss << "{";
                oss << "\"action\":\"" << EscapeJsonString(entry.action) << "\",";
                oss << "\"context\":\"" << EscapeJsonString(entry.context) << "\",";
                oss << "\"details\":\"" << EscapeJsonString(entry.details) << "\",";
                oss << "\"timestamp\":\"" << EscapeJsonString(entry.timestamp) << "\",";
                oss << "\"sequenceId\":" << entry.sequenceId;
                oss << "}";
            }
        }

        oss << "],";
        oss << "\"systemInfo\":" << DebugStackTrace::GetSystemInfo().ToJson();
        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleDebugContext() {
        m_requestCount++;
        APIResponse response;

        std::ostringstream oss;
        oss << "{";

        // Get system info
        auto sysInfo = DebugStackTrace::GetSystemInfo();
        oss << "\"systemInfo\":" << sysInfo.ToJson() << ",";

        // Check if debugger is present
        oss << "\"debuggerPresent\":" << (DebugStackTrace::IsDebuggerPresent() ? "true" : "false") << ",";

        // Get current thread ID
        oss << "\"currentThreadId\":" << DebugStackTrace::GetCurrentThreadId() << ",";

        // Get loaded modules
        auto modules = DebugStackTrace::GetLoadedModules();
        oss << "\"loadedModules\":[";
        bool first = true;
        for (const auto& mod : modules) {
            if (!first) oss << ",";
            first = false;
            oss << "{";
            oss << "\"name\":\"" << EscapeJsonString(mod.name) << "\",";
            oss << "\"path\":\"" << EscapeJsonString(mod.path) << "\",";
            oss << "\"baseAddress\":\"0x" << std::hex << reinterpret_cast<uintptr_t>(mod.baseAddress) << std::dec << "\",";
            oss << "\"size\":" << mod.size;
            oss << "}";
        }
        oss << "]";

        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleDebugHistory(const std::string& queryParams) {
        m_requestCount++;
        APIResponse response;

        std::string countStr = ParseQueryParam(queryParams, "count");
        int count = countStr.empty() ? 20 : std::stoi(countStr);

        auto recentActions = DebugStackTrace::GetRecentActions(count);

        std::ostringstream oss;
        oss << "{\"history\":[";

        bool first = true;
        for (const auto& entry : recentActions) {
            if (!first) oss << ",";
            first = false;

            oss << "{";
            oss << "\"action\":\"" << EscapeJsonString(entry.action) << "\",";
            oss << "\"context\":\"" << EscapeJsonString(entry.context) << "\",";
            oss << "\"details\":\"" << EscapeJsonString(entry.details) << "\",";
            oss << "\"timestamp\":\"" << EscapeJsonString(entry.timestamp) << "\",";
            oss << "\"sequenceId\":" << entry.sequenceId;
            oss << "}";
        }

        oss << "],\"count\":" << recentActions.size() << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse EditorAPIServer::HandleDebugTriggerDump(const std::string& body) {
        m_requestCount++;
        APIResponse response;

        std::string reason = ParseJsonValue(body, "reason");
        if (reason.empty()) {
            reason = "Manual API trigger";
        }

        ACTION_LOG_PARAMS(ActionCategory::DEBUG_INFO, ActionSeverity::INFO,
                         "API: Debug dump triggered",
                         ActionParams().Set("reason", reason));

        // Trigger a debug dump with the provided reason
        ActionData contextData;
        contextData.Set("source", "API");
        contextData.Set("endpoint", "/api/debug/dump");

        DebugStackTrace::DumpDebugContext(reason, contextData);

        response.body = "{\"status\":\"ok\",\"message\":\"Debug dump triggered\",\"reason\":\"" +
                       EscapeJsonString(reason) + "\"}";
        return response;
    }

}
