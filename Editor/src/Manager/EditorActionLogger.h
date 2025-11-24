#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <functional>
#include <map>

namespace CSEditor {

    class ELogMgr;

    /**
     * @brief Action categories for editor logging
     */
    enum class ActionCategory {
        GAMEOBJECT,     // GameObject create, delete, duplicate, rename
        COMPONENT,      // Component add, remove, modify
        TRANSFORM,      // Transform position, rotation, scale changes
        SCENE,          // Scene load, save, new, play, stop
        SELECTION,      // Selection changes in hierarchy
        ASSET,          // Asset operations (import, delete, etc.)
        INSPECTOR,      // Inspector property changes
        EDITOR,         // General editor operations (menu, shortcuts, etc.)
        SYSTEM,         // System events (init, shutdown, errors)
        DEBUG_INFO,     // Debug/crash information
        INPUT           // User input events
    };

    /**
     * @brief Severity levels for logging
     */
    enum class ActionSeverity {
        DEBUG = 0,      // Detailed debug information
        INFO = 1,       // Normal information
        WARNING = 2,    // Warning messages
        ERR = 3,        // Error messages (renamed from ERROR to avoid Windows macro conflict)
        CRITICAL = 4    // Critical errors / crashes
    };

    /**
     * @brief Structured action data for MCP-style communication
     * Lightweight version directly in this header to avoid circular dependencies
     */
    struct ActionParams {
        std::map<std::string, std::string> data;

        ActionParams& Set(const std::string& key, const std::string& value) {
            data[key] = value;
            return *this;
        }

        ActionParams& Set(const std::string& key, int value) {
            data[key] = std::to_string(value);
            return *this;
        }

        ActionParams& Set(const std::string& key, float value) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(4) << value;
            data[key] = oss.str();
            return *this;
        }

        ActionParams& Set(const std::string& key, bool value) {
            data[key] = value ? "true" : "false";
            return *this;
        }

        ActionParams& Set(const std::string& key, const char* value) {
            data[key] = value ? value : "null";
            return *this;
        }

        template<typename T>
        ActionParams& SetPtr(const std::string& key, const T* ptr) {
            std::ostringstream oss;
            oss << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
            data[key] = oss.str();
            return *this;
        }

        std::string Get(const std::string& key) const {
            auto it = data.find(key);
            return (it != data.end()) ? it->second : "";
        }

        bool Has(const std::string& key) const {
            return data.find(key) != data.end();
        }

        bool IsEmpty() const { return data.empty(); }

        std::string ToJson() const {
            std::ostringstream oss;
            oss << "{";
            bool first = true;
            for (const auto& pair : data) {
                if (!first) oss << ",";
                first = false;
                oss << "\"" << pair.first << "\":\"" << pair.second << "\"";
            }
            oss << "}";
            return oss.str();
        }

        std::string ToLogString() const {
            std::ostringstream oss;
            bool first = true;
            for (const auto& pair : data) {
                if (!first) oss << ", ";
                first = false;
                oss << pair.first << "=" << pair.second;
            }
            return oss.str();
        }
    };

    /**
     * @brief Structure representing a single log entry with structured data
     */
    struct ActionLogEntry {
        std::string timestamp;
        unsigned long sequenceId = 0;       // Unique sequence number
        ActionCategory category;
        ActionSeverity severity;
        std::string action;
        std::string details;
        std::string context;                // Additional context (file path, object name, etc.)
        ActionParams params;                // Structured parameters
        std::string stackTrace;             // Optional stack trace (for errors)
        unsigned long threadId = 0;         // Thread ID

        // JSON export for MCP-style communication
        std::string ToJson() const;
    };

    /**
     * @brief Callback type for log event listeners (MCP-style)
     */
    using LogEventCallback = std::function<void(const ActionLogEntry&)>;

    /**
     * @brief Editor Action Logger for tracking all editor operations
     *
     * This class provides comprehensive logging for all editor actions
     * with MCP-style structured data support for external tool integration.
     */
    class EditorActionLogger {
    public:
        static EditorActionLogger& GetInstance();

        // Delete copy constructor and assignment operator
        EditorActionLogger(const EditorActionLogger&) = delete;
        EditorActionLogger& operator=(const EditorActionLogger&) = delete;

        /**
         * @brief Initialize the logger
         * @param logFilePath Path to save log file (empty for no file logging)
         */
        void Init(const std::string& logFilePath = "");

        /**
         * @brief Shutdown the logger and flush all buffers
         */
        void Shutdown();

        /**
         * @brief Log an action with structured parameters
         */
        void LogWithParams(ActionCategory category, ActionSeverity severity,
                          const std::string& action, const ActionParams& params,
                          const std::string& context = "");

        /**
         * @brief Log an action (legacy interface)
         */
        void Log(ActionCategory category, ActionSeverity severity,
                const std::string& action, const std::string& details = "",
                const std::string& context = "");

        /**
         * @brief Log an error with stack trace
         */
        void LogError(const std::string& action, const std::string& errorMessage,
                     bool captureStack = true);

        /**
         * @brief Log a crash event
         */
        void LogCrash(const std::string& errorType, const std::string& errorMessage,
                     const std::string& stackTrace);

        // Convenience methods with structured parameters
        void LogGameObjectEx(const std::string& action, const ActionParams& params);
        void LogComponentEx(const std::string& action, const ActionParams& params);
        void LogTransformEx(const std::string& action, const ActionParams& params);
        void LogSceneEx(const std::string& action, const ActionParams& params);

        // Legacy convenience methods
        void LogGameObject(const std::string& action, const std::string& objectName,
                          const std::string& details = "");
        void LogComponent(const std::string& action, const std::string& componentType,
                         const std::string& objectName, const std::string& details = "");
        void LogTransform(const std::string& objectName, const std::string& property,
                         const std::string& oldValue, const std::string& newValue);
        void LogScene(const std::string& action, const std::string& scenePath = "");
        void LogSelection(const std::string& objectName, bool selected);
        void LogAsset(const std::string& action, const std::string& assetPath,
                     const std::string& details = "");
        void LogInspector(const std::string& action, const std::string& objectName,
                         const std::string& propertyName, const std::string& value = "");
        void LogEditor(const std::string& action, const std::string& details = "");
        void LogSystem(ActionSeverity severity, const std::string& action,
                      const std::string& details = "");
        void LogInput(const std::string& inputType, const ActionParams& params);

        /**
         * @brief Get all log entries
         */
        const std::vector<ActionLogEntry>& GetLogEntries() const { return m_logEntries; }

        /**
         * @brief Get entries as JSON array (for MCP export)
         */
        std::string GetEntriesAsJson(size_t lastN = 0) const;

        /**
         * @brief Register a callback for log events (MCP-style listener)
         */
        void RegisterEventListener(LogEventCallback callback);

        /**
         * @brief Clear event listeners
         */
        void ClearEventListeners();

        void ClearLogs();
        bool SaveToFile(const std::string& filePath = "");

        void SetConsoleOutputEnabled(bool enabled) { m_consoleOutputEnabled = enabled; }
        void SetFileLoggingEnabled(bool enabled) { m_fileLoggingEnabled = enabled; }
        void SetMinimumSeverity(ActionSeverity severity) { m_minimumSeverity = severity; }
        void SetJsonLogging(bool enabled) { m_jsonLogging = enabled; }

        const std::string& GetSessionStartTime() const { return m_sessionStartTime; }
        unsigned long GetCurrentSequenceId() const { return m_sequenceId; }

    private:
        EditorActionLogger();
        ~EditorActionLogger();

        std::string GetTimestamp() const;
        std::string CategoryToString(ActionCategory category) const;
        std::string SeverityToString(ActionSeverity severity) const;
        void WriteToConsole(const ActionLogEntry& entry) const;
        void WriteToFile(const ActionLogEntry& entry);
        std::string FormatLogEntry(const ActionLogEntry& entry) const;
        void NotifyListeners(const ActionLogEntry& entry);
        unsigned long GetThreadId() const;

    private:
        std::vector<ActionLogEntry> m_logEntries;
        std::vector<LogEventCallback> m_eventListeners;
        std::ofstream m_logFile;
        std::ofstream m_jsonLogFile;
        std::string m_logFilePath;
        std::string m_sessionStartTime;
        unsigned long m_sequenceId = 0;

        bool m_initialized = false;
        bool m_consoleOutputEnabled = true;
        bool m_fileLoggingEnabled = true;
        bool m_jsonLogging = true;
        ActionSeverity m_minimumSeverity = ActionSeverity::DEBUG;

        static constexpr size_t MAX_LOG_ENTRIES = 10000;
    };

    // Convenience macros for logging
    #define ACTION_LOG(category, severity, action, ...) \
        CSEditor::EditorActionLogger::GetInstance().Log(category, severity, action, ##__VA_ARGS__)

    #define ACTION_LOG_PARAMS(category, severity, action, params) \
        CSEditor::EditorActionLogger::GetInstance().LogWithParams(category, severity, action, params)

    #define ACTION_LOG_GAMEOBJECT(action, objectName, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogGameObject(action, objectName, ##__VA_ARGS__)

    #define ACTION_LOG_GAMEOBJECT_EX(action, params) \
        CSEditor::EditorActionLogger::GetInstance().LogGameObjectEx(action, params)

    #define ACTION_LOG_COMPONENT(action, componentType, objectName, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogComponent(action, componentType, objectName, ##__VA_ARGS__)

    #define ACTION_LOG_COMPONENT_EX(action, params) \
        CSEditor::EditorActionLogger::GetInstance().LogComponentEx(action, params)

    #define ACTION_LOG_TRANSFORM(objectName, property, oldValue, newValue) \
        CSEditor::EditorActionLogger::GetInstance().LogTransform(objectName, property, oldValue, newValue)

    #define ACTION_LOG_TRANSFORM_EX(action, params) \
        CSEditor::EditorActionLogger::GetInstance().LogTransformEx(action, params)

    #define ACTION_LOG_SCENE(action, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogScene(action, ##__VA_ARGS__)

    #define ACTION_LOG_SCENE_EX(action, params) \
        CSEditor::EditorActionLogger::GetInstance().LogSceneEx(action, params)

    #define ACTION_LOG_SELECTION(objectName, selected) \
        CSEditor::EditorActionLogger::GetInstance().LogSelection(objectName, selected)

    #define ACTION_LOG_ASSET(action, assetPath, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogAsset(action, assetPath, ##__VA_ARGS__)

    #define ACTION_LOG_INSPECTOR(action, objectName, propertyName, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogInspector(action, objectName, propertyName, ##__VA_ARGS__)

    #define ACTION_LOG_EDITOR(action, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogEditor(action, ##__VA_ARGS__)

    #define ACTION_LOG_SYSTEM(severity, action, ...) \
        CSEditor::EditorActionLogger::GetInstance().LogSystem(severity, action, ##__VA_ARGS__)

    #define ACTION_LOG_ERROR(action, errorMsg) \
        CSEditor::EditorActionLogger::GetInstance().LogError(action, errorMsg, true)

    #define ACTION_LOG_INPUT(inputType, params) \
        CSEditor::EditorActionLogger::GetInstance().LogInput(inputType, params)

}
