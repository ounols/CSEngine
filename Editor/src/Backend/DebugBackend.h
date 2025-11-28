#pragma once

#include <string>

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Backend logic for Debug operations
     *
     * Handles crash info, context, history, and debug dump.
     * Can be used by both REST API and Editor GUI.
     */
    class DebugBackend {
    public:
        static DebugBackend& GetInstance();

        DebugBackend(const DebugBackend&) = delete;
        DebugBackend& operator=(const DebugBackend&) = delete;

        // Debug operations
        APIResponse GetCrashInfo();
        APIResponse GetContext();
        APIResponse GetHistory(const std::string& queryParams);
        APIResponse TriggerDump(const std::string& body);

    private:
        DebugBackend() = default;
        ~DebugBackend() = default;
    };

}
