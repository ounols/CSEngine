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

        // ============================================
        // API Operations (async, uses pending queue)
        // ============================================
        APIResponse GetEditorInfo();
        APIResponse ExecuteCommand(const std::string& body);
        APIResponse CapturePreview(const std::string& queryParams);

        // Process pending operations on main thread
        void ProcessPendingOperations();

        // Accessors for port (set by EditorAPIServer)
        void SetApiPort(int port) { m_apiPort = port; }
        int GetApiPort() const { return m_apiPort; }

        // ============================================
        // Direct Operations (sync, for GUI use)
        // ============================================

        /**
         * @brief Start preview mode directly (synchronous)
         * @param width Preview window width
         * @param height Preview window height
         * @return true on success, false if already playing
         */
        bool PlayDirect(int width, int height);

        /**
         * @brief Stop preview mode directly (synchronous)
         * @return true on success, false if not playing
         */
        bool StopDirect();

        /**
         * @brief Resize preview directly (synchronous)
         * @param width New width
         * @param height New height
         */
        void ResizePreviewDirect(int width, int height);

        /**
         * @brief Check if preview is currently playing
         * @return true if in preview mode, false otherwise
         */
        bool IsPlaying();

        /**
         * @brief Get the preview texture ID
         * @return OpenGL texture ID or 0 if not available
         */
        unsigned int GetPreviewTextureId();

        /**
         * @brief Invoke editor render refresh
         */
        void InvokeEditorRender();

    private:
        EditorBackend() = default;
        ~EditorBackend() = default;

        int m_apiPort = 8080;

        std::mutex m_previewCaptureMutex;
        std::queue<PendingPreviewCapture> m_pendingPreviewCaptures;
    };

}
