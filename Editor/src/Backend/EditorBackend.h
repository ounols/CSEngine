#pragma once

#include <string>
#include <queue>
#include <mutex>

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Pending preview capture data
     */
    struct PendingPreviewCapture {
        std::string filename;
        std::string* resultPath;
        int* resultWidth;
        int* resultHeight;
        bool* success;
    };

    /**
     * @brief Backend logic for Editor operations
     *
     * Handles editor info, commands (play/stop), and preview capture.
     * Can be used by both REST API and Editor GUI.
     */
    class EditorBackend {
    public:
        static EditorBackend& GetInstance();

        EditorBackend(const EditorBackend&) = delete;
        EditorBackend& operator=(const EditorBackend&) = delete;

        // Editor operations
        APIResponse GetEditorInfo();
        APIResponse ExecuteCommand(const std::string& body);
        APIResponse CapturePreview(const std::string& queryParams);

        // Process pending operations on main thread
        void ProcessPendingOperations();

        // Accessors for port (set by EditorAPIServer)
        void SetApiPort(int port) { m_apiPort = port; }
        int GetApiPort() const { return m_apiPort; }

    private:
        EditorBackend() = default;
        ~EditorBackend() = default;

        int m_apiPort = 8080;

        std::mutex m_previewCaptureMutex;
        std::queue<PendingPreviewCapture> m_pendingPreviewCaptures;
    };

}
