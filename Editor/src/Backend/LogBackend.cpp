#include "LogBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"

namespace CSEditor {

    LogBackend& LogBackend::GetInstance() {
        static LogBackend instance;
        return instance;
    }

    APIResponse LogBackend::GetRecentLogs(const std::string& queryParams) {
        APIResponse response;

        std::string countStr = BackendUtils::ParseQueryParam(queryParams, "count");
        size_t count = countStr.empty() ? 20 : std::stoul(countStr);

        auto& logger = EditorActionLogger::GetInstance();
        response.body = logger.GetEntriesAsJson(count);
        return response;
    }

    APIResponse LogBackend::ClearLogs() {
        APIResponse response;

        EditorActionLogger::GetInstance().ClearLogs();

        ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::INFO, "API: Logs cleared", "");

        response.body = "{\"status\":\"ok\"}";
        return response;
    }

}
