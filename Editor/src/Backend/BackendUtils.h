#pragma once

#include <string>
#include <sstream>

namespace CSE {
    class SGameObject;
}

namespace CSEditor {

    /**
     * @brief Utility functions shared across all Backend classes
     */
    class BackendUtils {
    public:
        // JSON parsing utilities
        static std::string ParseJsonValue(const std::string& json, const std::string& key);
        static std::string ParseQueryParam(const std::string& query, const std::string& key);
        static std::string EscapeJsonString(const std::string& str);

        // GameObject utilities
        static std::string GameObjectToJson(CSE::SGameObject* obj, bool includeChildren = false);
        static CSE::SGameObject* FindGameObjectByName(const std::string& name);
    };

}
