#include "DebugBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"
#include "../Manager/DebugStackTrace.h"

#include <sstream>
#include <iomanip>

namespace CSEditor {

    DebugBackend& DebugBackend::GetInstance() {
        static DebugBackend instance;
        return instance;
    }

    APIResponse DebugBackend::GetCrashInfo() {
        APIResponse response;

        auto recentActions = DebugStackTrace::GetRecentActions(50);

        std::ostringstream oss;
        oss << "{\"crashHistory\":[";

        bool first = true;
        for (const auto& entry : recentActions) {
            if (entry.action.find("CRASH") != std::string::npos ||
                entry.action.find("ERROR") != std::string::npos ||
                entry.action.find("Exception") != std::string::npos ||
                entry.action.find("SIGSEGV") != std::string::npos ||
                entry.action.find("ACCESS_VIOLATION") != std::string::npos) {

                if (!first) oss << ",";
                first = false;

                oss << "{";
                oss << "\"action\":\"" << BackendUtils::EscapeJsonString(entry.action) << "\",";
                oss << "\"context\":\"" << BackendUtils::EscapeJsonString(entry.context) << "\",";
                oss << "\"details\":\"" << BackendUtils::EscapeJsonString(entry.details) << "\",";
                oss << "\"timestamp\":\"" << BackendUtils::EscapeJsonString(entry.timestamp) << "\",";
                oss << "\"sequenceId\":" << entry.sequenceId;
                oss << "}";
            }
        }

        oss << "],";
        oss << "\"systemInfo\":" << DebugStackTrace::GetSystemInfo().ToJson();
        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse DebugBackend::GetContext() {
        APIResponse response;

        std::ostringstream oss;
        oss << "{";

        auto sysInfo = DebugStackTrace::GetSystemInfo();
        oss << "\"systemInfo\":" << sysInfo.ToJson() << ",";

        oss << "\"debuggerPresent\":" << (DebugStackTrace::IsDebuggerPresent() ? "true" : "false") << ",";
        oss << "\"currentThreadId\":" << DebugStackTrace::GetCurrentThreadId() << ",";

        auto modules = DebugStackTrace::GetLoadedModules();
        oss << "\"loadedModules\":[";
        bool first = true;
        for (const auto& mod : modules) {
            if (!first) oss << ",";
            first = false;
            oss << "{";
            oss << "\"name\":\"" << BackendUtils::EscapeJsonString(mod.name) << "\",";
            oss << "\"path\":\"" << BackendUtils::EscapeJsonString(mod.path) << "\",";
            oss << "\"baseAddress\":\"0x" << std::hex << reinterpret_cast<uintptr_t>(mod.baseAddress) << std::dec << "\",";
            oss << "\"size\":" << mod.size;
            oss << "}";
        }
        oss << "]";

        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse DebugBackend::GetHistory(const std::string& queryParams) {
        APIResponse response;

        std::string countStr = BackendUtils::ParseQueryParam(queryParams, "count");
        int count = countStr.empty() ? 20 : std::stoi(countStr);

        auto recentActions = DebugStackTrace::GetRecentActions(count);

        std::ostringstream oss;
        oss << "{\"history\":[";

        bool first = true;
        for (const auto& entry : recentActions) {
            if (!first) oss << ",";
            first = false;

            oss << "{";
            oss << "\"action\":\"" << BackendUtils::EscapeJsonString(entry.action) << "\",";
            oss << "\"context\":\"" << BackendUtils::EscapeJsonString(entry.context) << "\",";
            oss << "\"details\":\"" << BackendUtils::EscapeJsonString(entry.details) << "\",";
            oss << "\"timestamp\":\"" << BackendUtils::EscapeJsonString(entry.timestamp) << "\",";
            oss << "\"sequenceId\":" << entry.sequenceId;
            oss << "}";
        }

        oss << "],\"count\":" << recentActions.size() << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse DebugBackend::TriggerDump(const std::string& body) {
        APIResponse response;

        std::string reason = BackendUtils::ParseJsonValue(body, "reason");
        if (reason.empty()) {
            reason = "Manual API trigger";
        }

        ACTION_LOG_PARAMS(ActionCategory::DEBUG_INFO, ActionSeverity::INFO,
                         "API: Debug dump triggered",
                         ActionParams().Set("reason", reason));

        ActionData contextData;
        contextData.Set("source", "API");
        contextData.Set("endpoint", "/api/debug/dump");

        DebugStackTrace::DumpDebugContext(reason, contextData);

        response.body = "{\"status\":\"ok\",\"message\":\"Debug dump triggered\",\"reason\":\"" +
                       BackendUtils::EscapeJsonString(reason) + "\"}";
        return response;
    }

}
