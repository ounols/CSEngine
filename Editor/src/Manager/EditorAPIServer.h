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
     * Runs on a separate thread and queues requests for main thread processing.
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

    private:
        EditorAPIServer();
        ~EditorAPIServer();

        // Server thread function
        void ServerThread();

        // Request handlers (called on main thread via queue)
        APIResponse HandleRequest(const APIRequest& request);

        // API endpoint handlers
        APIResponse HandleSceneInfo();
        APIResponse HandleSceneList();
        APIResponse HandleSceneLoad(const std::string& body);
        APIResponse HandleSceneSave();

        APIResponse HandleObjectCreate(const std::string& body);
        APIResponse HandleObjectDelete(const std::string& body);
        APIResponse HandleObjectSelect(const std::string& queryParams);
        APIResponse HandleObjectList();
        APIResponse HandleObjectInfo(const std::string& queryParams);
        APIResponse HandleTransformSet(const std::string& body);

        APIResponse HandleComponentAdd(const std::string& body);
        APIResponse HandleComponentRemove(const std::string& body);
        APIResponse HandleComponentList(const std::string& queryParams);

        APIResponse HandleLogRecent(const std::string& queryParams);
        APIResponse HandleLogClear();

        APIResponse HandleEditorInfo();
        APIResponse HandleEditorCommand(const std::string& body);
        
        APIResponse HandleCapturePreview(const std::string& queryParams);

        // Debug/Crash endpoints
        APIResponse HandleDebugCrashInfo();
        APIResponse HandleDebugContext();
        APIResponse HandleDebugHistory(const std::string& queryParams);
        APIResponse HandleDebugTriggerDump(const std::string& body);

        // Utility functions
        std::string ParseJsonValue(const std::string& json, const std::string& key) const;
        std::string ParseQueryParam(const std::string& query, const std::string& key) const;
        std::string EscapeJsonString(const std::string& str) const;
        std::string GameObjectToJson(CSE::SGameObject* obj, bool includeChildren = false) const;
        CSE::SGameObject* FindGameObjectByName(const std::string& name) const;

        // Error handling - returns debug info when errors occur
        APIResponse BuildErrorResponseWithDebugInfo(const std::string& error, const std::string& endpoint);

    public:
        /**
         * @brief Check and execute pending main thread commands (scene load, etc.)
         * Should be called from the main update loop
         */
        void ProcessMainThreadCommands();

        /**
         * @brief Check if there's a pending scene to load
         */
        bool HasPendingSceneLoad() const { return !m_pendingScenePath.empty(); }

        /**
         * @brief Get and clear the pending scene path
         */
        std::string ConsumePendingScenePath();

    private:
        std::thread m_serverThread;
        std::atomic<bool> m_running{false};
        std::atomic<bool> m_shouldStop{false};
        int m_port = 8080;

        // Request queue for main thread processing
        std::mutex m_requestMutex;
        std::queue<std::pair<APIRequest, std::function<void(const APIResponse&)>>> m_pendingRequests;

    // Pending scene load/save (to be processed on main thread)
    std::mutex m_sceneMutex;
    std::string m_pendingScenePath;
    bool m_pendingSceneSave = false;
    std::string m_sceneSavePath;

    // Pending object operations
    struct PendingObjectCreate {
        std::string type;
        std::string name;
    };
    struct PendingObjectDelete {
        std::string name;
    };
    struct PendingComponentAdd {
        std::string objectName;
        std::string componentType;
        std::string scriptPath;  // Optional, for CustomComponent
    };
    struct PendingComponentRemove {
        std::string objectName;
        std::string componentType;
    };
    struct PendingTransformSet {
        std::string objectName;
        float posX, posY, posZ;
        float rotX, rotY, rotZ, rotW;
        float scaleX, scaleY, scaleZ;
        bool setPosition;
        bool setRotation;
        bool setScale;
    };
    struct PendingPreviewCapture {
        std::string filename;
        std::string* resultPath;      // Output parameter
        int* resultWidth;              // Output parameter
        int* resultHeight;             // Output parameter
        bool* success;                 // Output parameter
    };

    std::mutex m_objectMutex;
    std::queue<PendingObjectCreate> m_pendingObjectCreates;
    std::queue<PendingObjectDelete> m_pendingObjectDeletes;
    
    std::mutex m_componentMutex;
    std::queue<PendingComponentAdd> m_pendingComponentAdds;
    std::queue<PendingComponentRemove> m_pendingComponentRemoves;
    
    std::mutex m_transformMutex;
    std::queue<PendingTransformSet> m_pendingTransformSets;
    
    std::mutex m_previewCaptureMutex;
    std::queue<PendingPreviewCapture> m_pendingPreviewCaptures;

    // Statistics
    std::atomic<unsigned long> m_requestCount{0};
    std::atomic<unsigned long> m_errorCount{0};
};

    // Convenience macro for checking API server
    #define API_SERVER CSEditor::EditorAPIServer::GetInstance()

}
