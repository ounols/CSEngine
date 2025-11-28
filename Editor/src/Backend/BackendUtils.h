#pragma once

#include <string>
#include <sstream>

namespace CSE {
    class SGameObject;
    class SScene;
}

namespace CSEditor {

    class EEngineCore;

    /**
     * @brief Utility functions shared across all Backend classes
     */
    class BackendUtils {
    public:
        // Core access utilities
        static EEngineCore* GetEditorCore();
        static CSE::SScene* GetCurrentScene();

        // JSON parsing utilities
        static std::string ParseJsonValue(const std::string& json, const std::string& key);
        static std::string ParseQueryParam(const std::string& query, const std::string& key);
        static std::string EscapeJsonString(const std::string& str);

        // GameObject utilities
        static std::string GameObjectToJson(const CSE::SGameObject* obj, bool includeChildren = false);
        static CSE::SGameObject* FindGameObjectByName(const std::string& name);
    };

}
