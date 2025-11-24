#pragma once

#include <string>
#include <mutex>

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Backend logic for Scene operations
     *
     * Handles scene loading, saving, and information retrieval.
     * Can be used by both REST API and Editor GUI.
     */
    class SceneBackend {
    public:
        static SceneBackend& GetInstance();

        SceneBackend(const SceneBackend&) = delete;
        SceneBackend& operator=(const SceneBackend&) = delete;

        // Scene operations
        APIResponse GetSceneInfo();
        APIResponse GetSceneList();
        APIResponse LoadScene(const std::string& body);
        APIResponse SaveScene(const std::string& body = "");

        // Pending scene operations (for main thread processing)
        bool HasPendingSceneLoad() const { return !m_pendingScenePath.empty(); }
        std::string ConsumePendingScenePath();

        bool HasPendingSceneSave() const { return m_pendingSceneSave; }
        std::string ConsumePendingSceneSavePath();

        // Process pending operations on main thread
        void ProcessPendingOperations();

    private:
        SceneBackend() = default;
        ~SceneBackend() = default;

        std::mutex m_sceneMutex;
        std::string m_pendingScenePath;
        bool m_pendingSceneSave = false;
        std::string m_sceneSavePath;
    };

}
