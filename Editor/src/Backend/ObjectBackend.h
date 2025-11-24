#pragma once

#include <string>
#include <queue>
#include <mutex>

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

        // Object operations
        APIResponse GetObjectList();
        APIResponse GetObjectInfo(const std::string& queryParams);
        APIResponse SelectObject(const std::string& queryParams);
        APIResponse CreateObject(const std::string& body);
        APIResponse DeleteObject(const std::string& body);
        APIResponse SetTransform(const std::string& body);

        // Process pending operations on main thread
        void ProcessPendingOperations();

    private:
        ObjectBackend() = default;
        ~ObjectBackend() = default;

        std::mutex m_objectMutex;
        std::queue<PendingObjectCreate> m_pendingObjectCreates;
        std::queue<PendingObjectDelete> m_pendingObjectDeletes;

        std::mutex m_transformMutex;
        std::queue<PendingTransformSet> m_pendingTransformSets;
    };

}
