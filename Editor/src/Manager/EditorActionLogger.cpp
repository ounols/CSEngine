#include "EditorActionLogger.h"
#include "EEngineCore.h"
#include "ELogMgr.h"
#include "DebugStackTrace.h"
#include <iostream>
#include <ctime>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

using namespace CSEditor;

// ActionLogEntry JSON export
std::string ActionLogEntry::ToJson() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"seq\":" << sequenceId << ",";
    oss << "\"timestamp\":\"" << timestamp << "\",";
    oss << "\"threadId\":" << threadId << ",";
    oss << "\"category\":\"" << static_cast<int>(category) << "\",";
    oss << "\"severity\":\"" << static_cast<int>(severity) << "\",";
    oss << "\"action\":\"" << action << "\"";

    if (!context.empty()) {
        oss << ",\"context\":\"" << context << "\"";
    }
    if (!details.empty()) {
        oss << ",\"details\":\"" << details << "\"";
    }
    if (!params.IsEmpty()) {
        oss << ",\"params\":" << params.ToJson();
    }
    if (!stackTrace.empty()) {
        // Escape newlines in stack trace
        std::string escaped = stackTrace;
        size_t pos = 0;
        while ((pos = escaped.find('\n', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\n");
            pos += 2;
        }
        oss << ",\"stackTrace\":\"" << escaped << "\"";
    }

    oss << "}";
    return oss.str();
}

EditorActionLogger& EditorActionLogger::GetInstance() {
    static EditorActionLogger instance;
    return instance;
}

EditorActionLogger::EditorActionLogger() = default;

EditorActionLogger::~EditorActionLogger() {
    Shutdown();
}

void EditorActionLogger::Init(const std::string& logFilePath) {
    if (m_initialized) return;

    m_sessionStartTime = GetTimestamp();
    m_logEntries.reserve(1000);
    m_sequenceId = 0;

    // Setup crash handler callback
    DebugStackTrace::SetCrashCallback([this](const DebugContext& ctx) {
        LogCrash(ctx.errorType, ctx.errorMessage,
                DebugStackTrace::FormatStackTrace(ctx.stackTrace));
    });

    // Install crash handlers
    DebugStackTrace::InstallCrashHandlers();

    // Create default log file path with timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream oss;
    oss << "editor_log_" << std::put_time(&tm, "%Y%m%d_%H%M%S");

    if (!logFilePath.empty()) {
        m_logFilePath = logFilePath;
    } else {
        m_logFilePath = oss.str() + ".log";
    }

    if (m_fileLoggingEnabled) {
        m_logFile.open(m_logFilePath, std::ios::out | std::ios::trunc);
        if (m_logFile.is_open()) {
            m_logFile << "=== CSEngine Editor Action Log ===" << std::endl;
            m_logFile << "Session started: " << m_sessionStartTime << std::endl;
            m_logFile << "=================================" << std::endl << std::endl;
            m_logFile.flush();
        }
    }

    // Open JSON log file for structured logging
    if (m_jsonLogging) {
        std::string jsonPath = oss.str() + ".json";
        m_jsonLogFile.open(jsonPath, std::ios::out | std::ios::trunc);
        if (m_jsonLogFile.is_open()) {
            m_jsonLogFile << "{\"session\":\"" << m_sessionStartTime << "\",\"entries\":[" << std::endl;
        }
    }

    m_initialized = true;
    LogSystem(ActionSeverity::INFO, "EditorActionLogger initialized",
             "Log file: " + m_logFilePath);
}

void EditorActionLogger::Shutdown() {
    if (!m_initialized) return;

    LogSystem(ActionSeverity::INFO, "EditorActionLogger shutting down",
             "Total entries: " + std::to_string(m_logEntries.size()));

    if (m_logFile.is_open()) {
        m_logFile << std::endl << "=== Session ended ===" << std::endl;
        m_logFile << "End time: " << GetTimestamp() << std::endl;
        m_logFile.close();
    }

    if (m_jsonLogFile.is_open()) {
        m_jsonLogFile << std::endl << "]}" << std::endl;
        m_jsonLogFile.close();
    }

    DebugStackTrace::UninstallCrashHandlers();
    m_eventListeners.clear();
    m_initialized = false;
}

unsigned long EditorActionLogger::GetThreadId() const {
#ifdef _WIN32
    return static_cast<unsigned long>(::GetCurrentThreadId());
#else
    return static_cast<unsigned long>(pthread_self());
#endif
}

std::string EditorActionLogger::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string EditorActionLogger::CategoryToString(ActionCategory category) const {
    switch (category) {
        case ActionCategory::GAMEOBJECT: return "GAMEOBJECT";
        case ActionCategory::COMPONENT:  return "COMPONENT";
        case ActionCategory::TRANSFORM:  return "TRANSFORM";
        case ActionCategory::SCENE:      return "SCENE";
        case ActionCategory::SELECTION:  return "SELECTION";
        case ActionCategory::ASSET:      return "ASSET";
        case ActionCategory::INSPECTOR:  return "INSPECTOR";
        case ActionCategory::EDITOR:     return "EDITOR";
        case ActionCategory::SYSTEM:     return "SYSTEM";
        case ActionCategory::DEBUG_INFO: return "DEBUG";
        case ActionCategory::INPUT:      return "INPUT";
        default: return "UNKNOWN";
    }
}

std::string EditorActionLogger::SeverityToString(ActionSeverity severity) const {
    switch (severity) {
        case ActionSeverity::DEBUG:    return "DEBUG";
        case ActionSeverity::INFO:     return "INFO";
        case ActionSeverity::WARNING:  return "WARN";
        case ActionSeverity::ERR:      return "ERROR";
        case ActionSeverity::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void EditorActionLogger::LogWithParams(ActionCategory category, ActionSeverity severity,
                                       const std::string& action, const ActionParams& params,
                                       const std::string& context) {
    if (static_cast<int>(severity) < static_cast<int>(m_minimumSeverity)) {
        return;
    }

    ActionLogEntry entry;
    entry.timestamp = GetTimestamp();
    entry.sequenceId = ++m_sequenceId;
    entry.threadId = GetThreadId();
    entry.category = category;
    entry.severity = severity;
    entry.action = action;
    entry.params = params;
    entry.context = context;
    entry.details = params.ToLogString();

    // Prevent memory overflow
    if (m_logEntries.size() >= MAX_LOG_ENTRIES) {
        m_logEntries.erase(m_logEntries.begin(), m_logEntries.begin() + 1000);
    }

    m_logEntries.push_back(entry);

    // Add to crash history buffer for debugging
    DebugStackTrace::AddToActionHistory(action, context, entry.details, entry.sequenceId);

    if (m_consoleOutputEnabled) {
        WriteToConsole(entry);
    }

    if (m_fileLoggingEnabled && m_logFile.is_open()) {
        WriteToFile(entry);
    }

    // Write to JSON log
    if (m_jsonLogging && m_jsonLogFile.is_open()) {
        if (m_sequenceId > 1) {
            m_jsonLogFile << "," << std::endl;
        }
        m_jsonLogFile << entry.ToJson();
        m_jsonLogFile.flush();
    }

    NotifyListeners(entry);
}

void EditorActionLogger::Log(ActionCategory category, ActionSeverity severity,
                            const std::string& action, const std::string& details,
                            const std::string& context) {
    if (static_cast<int>(severity) < static_cast<int>(m_minimumSeverity)) {
        return;
    }

    ActionLogEntry entry;
    entry.timestamp = GetTimestamp();
    entry.sequenceId = ++m_sequenceId;
    entry.threadId = GetThreadId();
    entry.category = category;
    entry.severity = severity;
    entry.action = action;
    entry.details = details;
    entry.context = context;

    // Prevent memory overflow
    if (m_logEntries.size() >= MAX_LOG_ENTRIES) {
        m_logEntries.erase(m_logEntries.begin(), m_logEntries.begin() + 1000);
    }

    m_logEntries.push_back(entry);

    // Add to crash history buffer for debugging
    DebugStackTrace::AddToActionHistory(action, context, details, entry.sequenceId);

    if (m_consoleOutputEnabled) {
        WriteToConsole(entry);
    }

    if (m_fileLoggingEnabled && m_logFile.is_open()) {
        WriteToFile(entry);
    }

    // Write to JSON log
    if (m_jsonLogging && m_jsonLogFile.is_open()) {
        if (m_sequenceId > 1) {
            m_jsonLogFile << "," << std::endl;
        }
        m_jsonLogFile << entry.ToJson();
        m_jsonLogFile.flush();
    }

    NotifyListeners(entry);
}

void EditorActionLogger::LogError(const std::string& action, const std::string& errorMessage,
                                  bool captureStack) {
    ActionLogEntry entry;
    entry.timestamp = GetTimestamp();
    entry.sequenceId = ++m_sequenceId;
    entry.threadId = GetThreadId();
    entry.category = ActionCategory::DEBUG_INFO;
    entry.severity = ActionSeverity::ERR;
    entry.action = action;
    entry.details = errorMessage;

    if (captureStack) {
        auto frames = DebugStackTrace::Capture(2, 32);
        entry.stackTrace = DebugStackTrace::FormatStackTrace(frames);
    }

    // Prevent memory overflow
    if (m_logEntries.size() >= MAX_LOG_ENTRIES) {
        m_logEntries.erase(m_logEntries.begin(), m_logEntries.begin() + 1000);
    }

    m_logEntries.push_back(entry);

    if (m_consoleOutputEnabled) {
        WriteToConsole(entry);
    }

    if (m_fileLoggingEnabled && m_logFile.is_open()) {
        WriteToFile(entry);
        if (!entry.stackTrace.empty()) {
            m_logFile << entry.stackTrace << std::endl;
        }
    }

    if (m_jsonLogging && m_jsonLogFile.is_open()) {
        if (m_sequenceId > 1) {
            m_jsonLogFile << "," << std::endl;
        }
        m_jsonLogFile << entry.ToJson();
        m_jsonLogFile.flush();
    }

    NotifyListeners(entry);
}

void EditorActionLogger::LogCrash(const std::string& errorType, const std::string& errorMessage,
                                  const std::string& stackTrace) {
    ActionLogEntry entry;
    entry.timestamp = GetTimestamp();
    entry.sequenceId = ++m_sequenceId;
    entry.threadId = GetThreadId();
    entry.category = ActionCategory::DEBUG_INFO;
    entry.severity = ActionSeverity::CRITICAL;
    entry.action = "CRASH: " + errorType;
    entry.details = errorMessage;
    entry.stackTrace = stackTrace;

    m_logEntries.push_back(entry);

    // Always write crashes to file
    if (m_logFile.is_open()) {
        m_logFile << "\n========== CRASH ==========\n";
        m_logFile << FormatLogEntry(entry) << std::endl;
        m_logFile << stackTrace << std::endl;
        m_logFile << "===========================\n";
        m_logFile.flush();
    }

    if (m_jsonLogFile.is_open()) {
        if (m_sequenceId > 1) {
            m_jsonLogFile << "," << std::endl;
        }
        m_jsonLogFile << entry.ToJson();
        m_jsonLogFile.flush();
    }

    NotifyListeners(entry);
}

void EditorActionLogger::LogGameObjectEx(const std::string& action, const ActionParams& params) {
    LogWithParams(ActionCategory::GAMEOBJECT, ActionSeverity::INFO, action, params,
                 params.Get("objectName"));
}

void EditorActionLogger::LogComponentEx(const std::string& action, const ActionParams& params) {
    std::string ctx = params.Get("objectName") + "::" + params.Get("componentType");
    LogWithParams(ActionCategory::COMPONENT, ActionSeverity::INFO, action, params, ctx);
}

void EditorActionLogger::LogTransformEx(const std::string& action, const ActionParams& params) {
    LogWithParams(ActionCategory::TRANSFORM, ActionSeverity::DEBUG, action, params,
                 params.Get("objectName"));
}

void EditorActionLogger::LogSceneEx(const std::string& action, const ActionParams& params) {
    LogWithParams(ActionCategory::SCENE, ActionSeverity::INFO, action, params,
                 params.Get("scenePath"));
}

void EditorActionLogger::LogGameObject(const std::string& action, const std::string& objectName,
                                       const std::string& details) {
    ActionParams params;
    params.Set("objectName", objectName);
    if (!details.empty()) params.Set("details", details);
    LogWithParams(ActionCategory::GAMEOBJECT, ActionSeverity::INFO, action, params, objectName);
}

void EditorActionLogger::LogComponent(const std::string& action, const std::string& componentType,
                                      const std::string& objectName, const std::string& details) {
    ActionParams params;
    params.Set("objectName", objectName);
    params.Set("componentType", componentType);
    if (!details.empty()) params.Set("details", details);
    std::string ctx = objectName + "::" + componentType;
    LogWithParams(ActionCategory::COMPONENT, ActionSeverity::INFO, action, params, ctx);
}

void EditorActionLogger::LogTransform(const std::string& objectName, const std::string& property,
                                      const std::string& oldValue, const std::string& newValue) {
    ActionParams params;
    params.Set("objectName", objectName);
    params.Set("property", property);
    params.Set("oldValue", oldValue);
    params.Set("newValue", newValue);
    LogWithParams(ActionCategory::TRANSFORM, ActionSeverity::DEBUG, "Transform changed",
                 params, objectName);
}

void EditorActionLogger::LogScene(const std::string& action, const std::string& scenePath) {
    ActionParams params;
    if (!scenePath.empty()) params.Set("scenePath", scenePath);
    LogWithParams(ActionCategory::SCENE, ActionSeverity::INFO, action, params, scenePath);
}

void EditorActionLogger::LogSelection(const std::string& objectName, bool selected) {
    ActionParams params;
    params.Set("objectName", objectName);
    params.Set("selected", selected);
    std::string action = selected ? "Object selected" : "Object deselected";
    LogWithParams(ActionCategory::SELECTION, ActionSeverity::DEBUG, action, params, objectName);
}

void EditorActionLogger::LogAsset(const std::string& action, const std::string& assetPath,
                                  const std::string& details) {
    ActionParams params;
    params.Set("assetPath", assetPath);
    if (!details.empty()) params.Set("details", details);
    LogWithParams(ActionCategory::ASSET, ActionSeverity::INFO, action, params, assetPath);
}

void EditorActionLogger::LogInspector(const std::string& action, const std::string& objectName,
                                      const std::string& propertyName, const std::string& value) {
    ActionParams params;
    params.Set("objectName", objectName);
    params.Set("propertyName", propertyName);
    if (!value.empty()) params.Set("value", value);
    LogWithParams(ActionCategory::INSPECTOR, ActionSeverity::DEBUG, action, params, objectName);
}

void EditorActionLogger::LogEditor(const std::string& action, const std::string& details) {
    ActionParams params;
    if (!details.empty()) params.Set("details", details);
    LogWithParams(ActionCategory::EDITOR, ActionSeverity::INFO, action, params, "");
}

void EditorActionLogger::LogSystem(ActionSeverity severity, const std::string& action,
                                   const std::string& details) {
    ActionParams params;
    if (!details.empty()) params.Set("details", details);
    LogWithParams(ActionCategory::SYSTEM, severity, action, params, "");
}

void EditorActionLogger::LogInput(const std::string& inputType, const ActionParams& params) {
    LogWithParams(ActionCategory::INPUT, ActionSeverity::DEBUG, inputType, params, "");
}

void EditorActionLogger::RegisterEventListener(LogEventCallback callback) {
    m_eventListeners.push_back(callback);
}

void EditorActionLogger::ClearEventListeners() {
    m_eventListeners.clear();
}

void EditorActionLogger::NotifyListeners(const ActionLogEntry& entry) {
    for (const auto& listener : m_eventListeners) {
        try {
            listener(entry);
        } catch (...) {
            // Ignore listener errors
        }
    }
}

std::string EditorActionLogger::GetEntriesAsJson(size_t lastN) const {
    std::ostringstream oss;
    oss << "[";

    size_t start = 0;
    if (lastN > 0 && lastN < m_logEntries.size()) {
        start = m_logEntries.size() - lastN;
    }

    bool first = true;
    for (size_t i = start; i < m_logEntries.size(); i++) {
        if (!first) oss << ",";
        first = false;
        oss << m_logEntries[i].ToJson();
    }

    oss << "]";
    return oss.str();
}

void EditorActionLogger::ClearLogs() {
    m_logEntries.clear();
    LogSystem(ActionSeverity::INFO, "Log entries cleared", "");
}

bool EditorActionLogger::SaveToFile(const std::string& filePath) {
    std::string targetPath = filePath.empty() ? m_logFilePath : filePath;

    std::ofstream outFile(targetPath, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        LogSystem(ActionSeverity::ERR, "Failed to save log file", targetPath);
        return false;
    }

    outFile << "=== CSEngine Editor Action Log ===" << std::endl;
    outFile << "Session started: " << m_sessionStartTime << std::endl;
    outFile << "Saved at: " << GetTimestamp() << std::endl;
    outFile << "Total entries: " << m_logEntries.size() << std::endl;
    outFile << "=================================" << std::endl << std::endl;

    for (const auto& entry : m_logEntries) {
        outFile << FormatLogEntry(entry) << std::endl;
        if (!entry.stackTrace.empty()) {
            outFile << entry.stackTrace << std::endl;
        }
    }

    outFile.close();
    LogSystem(ActionSeverity::INFO, "Log saved to file", targetPath);
    return true;
}

void EditorActionLogger::WriteToConsole(const ActionLogEntry& entry) const {
    const auto& editorCore = EEngineCore::getEditorInstance();
    if (editorCore == nullptr) return;
    if (editorCore->IsDestroyQueue()) return;

    const auto& logMgr = editorCore->GetLogMgrCore();
    if (logMgr == nullptr) return;

    const std::string logMessage = FormatLogEntry(entry);

    ELogMgr::Category logCategory;
    switch (entry.severity) {
        case ActionSeverity::WARNING:
            logCategory = ELogMgr::Category::WARNING_LOG;
            break;
        case ActionSeverity::ERR:
        case ActionSeverity::CRITICAL:
            logCategory = ELogMgr::Category::ERROR_LOG;
            break;
        default:
            logCategory = ELogMgr::Category::INFO_LOG;
            break;
    }

    logMgr->AddLog(logMessage.c_str(), logCategory);
}

void EditorActionLogger::WriteToFile(const ActionLogEntry& entry) {
    if (!m_logFile.is_open()) return;

    m_logFile << FormatLogEntry(entry) << std::endl;
    m_logFile.flush();
}

std::string EditorActionLogger::FormatLogEntry(const ActionLogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << entry.timestamp << "] "
        << "[#" << entry.sequenceId << "] "
        << "[" << SeverityToString(entry.severity) << "] "
        << "[" << CategoryToString(entry.category) << "] "
        << entry.action;

    if (!entry.context.empty()) {
        oss << " | Context: " << entry.context;
    }

    if (!entry.params.IsEmpty()) {
        oss << " | Params: " << entry.params.ToLogString();
    } else if (!entry.details.empty()) {
        oss << " | Details: " << entry.details;
    }

    return oss.str();
}
