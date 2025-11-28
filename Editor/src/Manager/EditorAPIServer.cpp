#include "EditorAPIServer.h"
#include "EditorActionLogger.h"
#include "EEngineCore.h"
#include "DebugStackTrace.h"

// Backend includes
#include "../Backend/BackendUtils.h"
#include "../Backend/SceneBackend.h"
#include "../Backend/ObjectBackend.h"
#include "../Backend/ComponentBackend.h"
#include "../Backend/LogBackend.h"
#include "../Backend/EditorBackend.h"
#include "../Backend/DebugBackend.h"

// Include httplib (header-only HTTP library)
#include "../Util/httplib.h"

#include <sstream>
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

        // Set port for EditorBackend
        EditorBackend::GetInstance().SetApiPort(port);

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
        oss << "\"error\":\"" << BackendUtils::EscapeJsonString(error) << "\",";
        oss << "\"endpoint\":\"" << BackendUtils::EscapeJsonString(endpoint) << "\",";

        // Include recent action history for debugging
        auto recentActions = DebugStackTrace::GetRecentActions(20);
        oss << "\"recentActions\":[";
        bool first = true;
        for (const auto& entry : recentActions) {
            if (!first) oss << ",";
            first = false;
            oss << "{";
            oss << "\"action\":\"" << BackendUtils::EscapeJsonString(entry.action) << "\",";
            oss << "\"context\":\"" << BackendUtils::EscapeJsonString(entry.context) << "\",";
            oss << "\"timestamp\":\"" << BackendUtils::EscapeJsonString(entry.timestamp) << "\"";
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
            oss << "\"function\":\"" << BackendUtils::EscapeJsonString(frame.function) << "\",";
            oss << "\"file\":\"" << BackendUtils::EscapeJsonString(frame.file) << "\",";
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
                m_requestCount++;
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

        // Scene endpoints - delegate to SceneBackend
        svr.Get("/api/scene/info", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/scene/info", []() { return SceneBackend::GetInstance().GetSceneInfo(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/scene/list", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/scene/list", []() { return SceneBackend::GetInstance().GetSceneList(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/scene/load", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/scene/load", [body]() { return SceneBackend::GetInstance().LoadScene(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/scene/save", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/scene/save", [body]() { return SceneBackend::GetInstance().SaveScene(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Object endpoints - delegate to ObjectBackend
        svr.Get("/api/object/list", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/object/list", []() { return ObjectBackend::GetInstance().GetObjectList(); });
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
            auto response = safeHandler("/api/object/info", [query]() { return ObjectBackend::GetInstance().GetObjectInfo(query); });
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
            auto response = safeHandler("/api/object/select", [query]() { return ObjectBackend::GetInstance().SelectObject(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/object/create", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/object/create", [body]() { return ObjectBackend::GetInstance().CreateObject(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/object/delete", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/object/delete", [body]() { return ObjectBackend::GetInstance().DeleteObject(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/object/set-transform", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/object/set-transform", [body]() { return ObjectBackend::GetInstance().SetTransform(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Component endpoints - delegate to ComponentBackend
        svr.Get("/api/component/list", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/component/list", [query]() { return ComponentBackend::GetInstance().GetComponentList(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/component/add", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/component/add", [body]() { return ComponentBackend::GetInstance().AddComponent(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/component/remove", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/component/remove", [body]() { return ComponentBackend::GetInstance().RemoveComponent(body); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Log endpoints - delegate to LogBackend
        svr.Get("/api/log/recent", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = safeHandler("/api/log/recent", [query]() { return LogBackend::GetInstance().GetRecentLogs(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/log/clear", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/log/clear", []() { return LogBackend::GetInstance().ClearLogs(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Editor endpoints - delegate to EditorBackend
        svr.Get("/api/editor/info", [this, &setCorsHeaders, &safeHandler](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            auto response = safeHandler("/api/editor/info", []() { return EditorBackend::GetInstance().GetEditorInfo(); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/editor/command", [this, &setCorsHeaders, &safeHandler](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            std::string body = req.body;
            auto response = safeHandler("/api/editor/command", [body]() { return EditorBackend::GetInstance().ExecuteCommand(body); });
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
            auto response = safeHandler("/api/editor/capture-preview", [query]() { return EditorBackend::GetInstance().CapturePreview(query); });
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        // Debug/Crash endpoints - delegate to DebugBackend (without safeHandler to avoid infinite recursion)
        svr.Get("/api/debug/crash", [this, &setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            m_requestCount++;
            auto response = DebugBackend::GetInstance().GetCrashInfo();
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/debug/context", [this, &setCorsHeaders](const httplib::Request&, httplib::Response& res) {
            setCorsHeaders(res);
            m_requestCount++;
            auto response = DebugBackend::GetInstance().GetContext();
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Get("/api/debug/history", [this, &setCorsHeaders](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            m_requestCount++;
            std::string query;
            for (const auto& param : req.params) {
                if (!query.empty()) query += "&";
                query += param.first + "=" + param.second;
            }
            auto response = DebugBackend::GetInstance().GetHistory(query);
            res.status = response.statusCode;
            res.set_content(response.body, response.contentType);
        });

        svr.Post("/api/debug/dump", [this, &setCorsHeaders](const httplib::Request& req, httplib::Response& res) {
            setCorsHeaders(res);
            m_requestCount++;
            auto response = DebugBackend::GetInstance().TriggerDump(req.body);
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

    void EditorAPIServer::ProcessMainThreadCommands() {
        // Delegate to all Backend classes
        SceneBackend::GetInstance().ProcessPendingOperations();
        ObjectBackend::GetInstance().ProcessPendingOperations();
        ComponentBackend::GetInstance().ProcessPendingOperations();
        EditorBackend::GetInstance().ProcessPendingOperations();
    }

    bool EditorAPIServer::HasPendingSceneLoad() const {
        return SceneBackend::GetInstance().HasPendingSceneLoad();
    }

    std::string EditorAPIServer::ConsumePendingScenePath() {
        return SceneBackend::GetInstance().ConsumePendingScenePath();
    }

}
