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
#include "../../../src/Manager/SceneMgr.h"
#include "../../../src/Manager/GameObjectMgr.h"

#include <sstream>
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
        if (path.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing path parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                         "API: Load Scene",
                         ActionParams().Set("path", path));

        // Queue scene load for main thread processing
        {
            std::lock_guard<std::mutex> lock(m_sceneMutex);
            m_pendingScenePath = path;
        }

        response.body = "{\"status\":\"queued\",\"path\":\"" + EscapeJsonString(path) + "\",\"message\":\"Scene will be loaded on next frame\"}";
        return response;
    }

    std::string EditorAPIServer::ConsumePendingScenePath() {
        std::lock_guard<std::mutex> lock(m_sceneMutex);
        std::string path = m_pendingScenePath;
        m_pendingScenePath.clear();
        return path;
    }

    void EditorAPIServer::ProcessMainThreadCommands() {
        // Process pending scene load
        if (HasPendingSceneLoad()) {
            std::string scenePath = ConsumePendingScenePath();
            if (!scenePath.empty()) {
                auto* core = EEngineCore::getEditorInstance();
                if (core) {
                    core->SetCurrentScene(scenePath);
                    ACTION_LOG_PARAMS(ActionCategory::SCENE, ActionSeverity::INFO,
                                     "API: Scene loaded",
                                     ActionParams().Set("path", scenePath));
                }
            }
        }
    }

    APIResponse EditorAPIServer::HandleSceneSave() {
        m_requestCount++;
        APIResponse response;

        ACTION_LOG(ActionCategory::SCENE, ActionSeverity::INFO, "API: Save Scene", "");

        response.body = "{\"status\":\"queued\"}";
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

        // Note: Object creation should be done on main thread
        // Return queued status
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
                core->InvokePreviewStart(800, 600);
                response.body = "{\"status\":\"ok\",\"command\":\"play\"}";
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
