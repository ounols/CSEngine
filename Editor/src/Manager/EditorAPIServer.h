#pragma once

#include <string>
#include <functional>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

namespace CSE {
    class SGameObject;
    class SScene;
}

namespace CSEditor {

    /**
     * @brief Request from the API server to be processed on the main thread
     */
    struct APIRequest {
        std::string endpoint;
        std::string method;
        std::string body;
        std::string queryParams;
        unsigned long requestId;
    };

    /**
     * @brief Response to be sent back to the client
     */
    struct APIResponse {
        int statusCode = 200;
        std::string contentType = "application/json";
        std::string body;
    };

    /**
     * @brief REST API Server for external tool integration
     *
     * Provides HTTP endpoints for controlling the editor via curl or other tools.
     * Runs on a separate thread and delegates actual logic to Backend classes.
     *
     * Backend classes handle the actual operations and can also be called
     * directly from the Editor GUI.
     *
     * Example usage:
     *   curl http://localhost:8080/api/scene/info
     *   curl -X POST http://localhost:8080/api/object/create -d '{"type":"empty","name":"MyObject"}'
     */
    class EditorAPIServer {
    public:
        static EditorAPIServer& GetInstance();

        // Delete copy constructor and assignment operator
        EditorAPIServer(const EditorAPIServer&) = delete;
        EditorAPIServer& operator=(const EditorAPIServer&) = delete;

        /**
         * @brief Start the API server
         * @param port Port number to listen on (default: 8080)
         * @return true if server started successfully
         */
        bool Start(int port = 8080);

        /**
         * @brief Stop the API server
         */
        void Stop();

        /**
         * @brief Check if server is running
         */
        bool IsRunning() const { return m_running; }

        /**
         * @brief Get the port number
         */
        int GetPort() const { return m_port; }

        /**
         * @brief Process pending requests on main thread
         * Should be called from the main update loop
         */
        void ProcessPendingRequests();

        /**
         * @brief Get server status as JSON
         */
        std::string GetStatusJson() const;

        /**
         * @brief Check and execute pending main thread commands
         * Delegates to Backend classes for actual processing
         * Should be called from the main update loop
         */
        void ProcessMainThreadCommands();

        /**
         * @brief Check if there's a pending scene to load
         * @deprecated Use SceneBackend::HasPendingSceneLoad() instead
         */
        bool HasPendingSceneLoad() const;

        /**
         * @brief Get and clear the pending scene path
         * @deprecated Use SceneBackend::ConsumePendingScenePath() instead
         */
        std::string ConsumePendingScenePath();

    private:
        EditorAPIServer();
        ~EditorAPIServer();

        // Server thread function
        void ServerThread();

        // Request handlers (called on main thread via queue)
        APIResponse HandleRequest(const APIRequest& request);

        // Error handling - returns debug info when errors occur
        APIResponse BuildErrorResponseWithDebugInfo(const std::string& error, const std::string& endpoint);

    private:
        std::thread m_serverThread;
        std::atomic<bool> m_running{false};
        std::atomic<bool> m_shouldStop{false};
        int m_port = 8080;

        // Request queue for main thread processing
        std::mutex m_requestMutex;
        std::queue<std::pair<APIRequest, std::function<void(const APIResponse&)>>> m_pendingRequests;

        // Statistics
        std::atomic<unsigned long> m_requestCount{0};
        std::atomic<unsigned long> m_errorCount{0};
    };

    // Convenience macro for checking API server
    #define API_SERVER CSEditor::EditorAPIServer::GetInstance()

}
