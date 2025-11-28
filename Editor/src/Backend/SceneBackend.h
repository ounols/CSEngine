#pragma once

#include <string>
#include <mutex>

namespace CSE {
    class SScene;
}

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

        // ============================================
        // API Operations (async, uses pending queue)
        // ============================================
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

        // ============================================
        // Direct Operations (sync, for GUI use)
        // ============================================

        /**
         * @brief Load a scene directly (synchronous)
         * @param path Full path to the scene file
         * @return true on success, false on failure
         */
        bool LoadSceneDirect(const std::string& path);

        /**
         * @brief Create a new empty scene directly (synchronous)
         * @return Pointer to the new scene or nullptr on failure
         */
        CSE::SScene* CreateNewSceneDirect();

        /**
         * @brief Save the current scene directly (synchronous)
         * @param path Path to save the scene (relative to Assets/)
         * @return true on success, false on failure
         */
        bool SaveSceneDirect(const std::string& path);

        /**
         * @brief Get the current scene name
         * @return Current scene name or empty string if no scene loaded
         */
        std::string GetCurrentSceneName();

    private:
        SceneBackend() = default;
        ~SceneBackend() = default;

        std::mutex m_sceneMutex;
        std::string m_pendingScenePath;
        bool m_pendingSceneSave = false;
        std::string m_sceneSavePath;
    };

}
