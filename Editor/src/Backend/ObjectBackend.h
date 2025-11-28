#pragma once

#include <string>
#include <queue>
#include <mutex>

namespace CSE {
    class SGameObject;
}

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Pending object creation data
     */
    struct PendingObjectCreate {
        std::string type;
        std::string name;
    };

    /**
     * @brief Pending object deletion data
     */
    struct PendingObjectDelete {
        std::string name;
    };

    /**
     * @brief Pending transform set data
     */
    struct PendingTransformSet {
        std::string objectName;
        float posX, posY, posZ;
        float rotX, rotY, rotZ, rotW;
        float scaleX, scaleY, scaleZ;
        bool setPosition;
        bool setRotation;
        bool setScale;
    };

    /**
     * @brief Backend logic for GameObject operations
     *
     * Handles object creation, deletion, listing, and transform operations.
     * Can be used by both REST API and Editor GUI.
     */
    class ObjectBackend {
    public:
        static ObjectBackend& GetInstance();

        ObjectBackend(const ObjectBackend&) = delete;
        ObjectBackend& operator=(const ObjectBackend&) = delete;

        // ============================================
        // API Operations (async, uses pending queue)
        // ============================================
        APIResponse GetObjectList();
        APIResponse GetObjectInfo(const std::string& queryParams);
        APIResponse SelectObject(const std::string& queryParams);
        APIResponse CreateObject(const std::string& body);
        APIResponse DeleteObject(const std::string& body);
        APIResponse SetTransform(const std::string& body);

        // Process pending operations on main thread
        void ProcessPendingOperations();

        // ============================================
        // Direct Operations (sync, for GUI use)
        // ============================================

        /**
         * @brief Create an empty GameObject directly (synchronous)
         * @param name Name for the new object
         * @param parent Parent object (nullptr for root)
         * @return Created GameObject or nullptr on failure
         */
        CSE::SGameObject* CreateEmptyObjectDirect(const std::string& name, CSE::SGameObject* parent = nullptr);

        /**
         * @brief Create a primitive GameObject directly (synchronous)
         * @param type Primitive type (e.g., "Cube", "Sphere", "Plane")
         * @param name Name for the new object
         * @param parent Parent object (nullptr for root)
         * @return Created GameObject or nullptr on failure
         */
        CSE::SGameObject* CreatePrimitiveObjectDirect(const std::string& type, const std::string& name, CSE::SGameObject* parent = nullptr);

        /**
         * @brief Create a light GameObject directly (synchronous)
         * @param lightType Light type ("Directional", "Point", "Spot")
         * @param parent Parent object (nullptr for root)
         * @return Created GameObject or nullptr on failure
         */
        CSE::SGameObject* CreateLightObjectDirect(const std::string& lightType, CSE::SGameObject* parent = nullptr);

        /**
         * @brief Create a camera GameObject directly (synchronous)
         * @param parent Parent object (nullptr for root)
         * @return Created GameObject or nullptr on failure
         */
        CSE::SGameObject* CreateCameraObjectDirect(CSE::SGameObject* parent = nullptr);

        /**
         * @brief Delete a GameObject directly (synchronous)
         * @param object Object to delete
         * @return true on success, false on failure
         */
        bool DeleteObjectDirect(CSE::SGameObject* object);

        /**
         * @brief Duplicate a GameObject directly (synchronous)
         * @param object Object to duplicate
         * @return Duplicated GameObject or nullptr on failure
         */
        CSE::SGameObject* DuplicateObjectDirect(CSE::SGameObject* object);

        /**
         * @brief Set transform of a GameObject directly (synchronous)
         * @param object Target object
         * @param position Position to set (nullptr to skip)
         * @param rotation Rotation to set as quaternion (nullptr to skip)
         * @param scale Scale to set (nullptr to skip)
         * @return true on success, false on failure
         */
        bool SetTransformDirect(CSE::SGameObject* object,
                               const float* position = nullptr,
                               const float* rotation = nullptr,
                               const float* scale = nullptr);

    private:
        ObjectBackend() = default;
        ~ObjectBackend() = default;

        // Helper to initialize and tick a newly created object
        void InitializeNewObject(CSE::SGameObject* obj);

        std::mutex m_objectMutex;
        std::queue<PendingObjectCreate> m_pendingObjectCreates;
        std::queue<PendingObjectDelete> m_pendingObjectDeletes;

        std::mutex m_transformMutex;
        std::queue<PendingTransformSet> m_pendingTransformSets;
    };

}
