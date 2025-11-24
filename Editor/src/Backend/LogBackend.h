#pragma once

#include <string>

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Backend logic for Log operations
     *
     * Handles log retrieval and clearing.
     * Can be used by both REST API and Editor GUI.
     */
    class LogBackend {
    public:
        static LogBackend& GetInstance();

        LogBackend(const LogBackend&) = delete;
        LogBackend& operator=(const LogBackend&) = delete;

        // Log operations
        APIResponse GetRecentLogs(const std::string& queryParams);
        APIResponse ClearLogs();

    private:
        LogBackend() = default;
        ~LogBackend() = default;
    };

}
