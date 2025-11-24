#pragma once

#include <string>
#include <queue>
#include <mutex>

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Pending component add data
     */
    struct PendingComponentAdd {
        std::string objectName;
        std::string componentType;
        std::string scriptPath;
    };

    /**
     * @brief Pending component remove data
     */
    struct PendingComponentRemove {
        std::string objectName;
        std::string componentType;
    };

    /**
     * @brief Backend logic for Component operations
     *
     * Handles component addition, removal, and listing.
     * Can be used by both REST API and Editor GUI.
     */
    class ComponentBackend {
    public:
        static ComponentBackend& GetInstance();

        ComponentBackend(const ComponentBackend&) = delete;
        ComponentBackend& operator=(const ComponentBackend&) = delete;

        // Component operations
        APIResponse GetComponentList(const std::string& queryParams);
        APIResponse AddComponent(const std::string& body);
        APIResponse RemoveComponent(const std::string& body);

        // Process pending operations on main thread
        void ProcessPendingOperations();

    private:
        ComponentBackend() = default;
        ~ComponentBackend() = default;

        std::mutex m_componentMutex;
        std::queue<PendingComponentAdd> m_pendingComponentAdds;
        std::queue<PendingComponentRemove> m_pendingComponentRemoves;
    };

}
