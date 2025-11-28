#pragma once

#include <string>
#include <queue>
#include <mutex>


class XNode;

namespace CSE {
    class SGameObject;
    class SComponent;
}

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Pending component add data
     */
    struct PendingComponentAdd {
        std::string objectName;
        std::string componentType;
        const XNode* node;
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

        // ============================================
        // API Operations (async, uses pending queue)
        // ============================================
        APIResponse GetComponentList(const std::string& queryParams);
        APIResponse AddComponent(const std::string& body);
        APIResponse RemoveComponent(const std::string& body);

        // Process pending operations on main thread
        void ProcessPendingOperations();

        // ============================================
        // Direct Operations (sync, for GUI use)
        // ============================================

        /**
         * @brief Add a component directly (synchronous)
         * @param object Target GameObject
         * @param componentType Type of component to add
         * @param node Optional script path for CustomComponent
         * @return Created component or nullptr on failure
         */
        CSE::SComponent* AddComponentDirect(CSE::SGameObject* object,
                                            const std::string& componentType,
                                            const XNode* node = nullptr);

        /**
         * @brief Remove a component directly (synchronous)
         * @param object Target GameObject
         * @param component Component to remove
         * @return true on success, false on failure
         */
        bool RemoveComponentDirect(CSE::SGameObject* object, CSE::SComponent* component);

        /**
         * @brief Check if a component type already exists on the object
         * @param object Target GameObject
         * @param componentType Type of component to check
         * @return true if component exists, false otherwise
         */
        static bool HasComponent(const CSE::SGameObject* object, const std::string& componentType);

    private:
        ComponentBackend() = default;
        ~ComponentBackend() = default;

        std::mutex m_componentMutex;
        std::queue<PendingComponentAdd> m_pendingComponentAdds;
        std::queue<PendingComponentRemove> m_pendingComponentRemoves;
    };

}
