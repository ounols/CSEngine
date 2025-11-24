#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <functional>
#include <cstdint>

namespace CSEditor {

    /**
     * @brief Represents a single stack frame
     */
    struct StackFrame {
        std::string function;       // Function name
        std::string file;           // Source file (if available)
        int line = 0;               // Line number (if available)
        void* address = nullptr;    // Instruction address
        std::string module;         // Module/library name
    };

    /**
     * @brief Structured action data for MCP-style communication
     * Key-value pairs that can represent any action's parameters
     */
    class ActionData {
    public:
        ActionData() = default;

        // Add various data types
        ActionData& Set(const std::string& key, const std::string& value);
        ActionData& Set(const std::string& key, int value);
        ActionData& Set(const std::string& key, float value);
        ActionData& Set(const std::string& key, double value);
        ActionData& Set(const std::string& key, bool value);
        ActionData& Set(const std::string& key, const char* value);

        // Add pointer/address info (for debugging)
        ActionData& SetPtr(const std::string& key, const void* ptr);

        // Add nested data
        ActionData& SetNested(const std::string& key, const ActionData& nested);

        // Add array of strings
        ActionData& SetArray(const std::string& key, const std::vector<std::string>& values);

        // Get value
        std::string Get(const std::string& key) const;
        bool Has(const std::string& key) const;

        // Serialize to JSON-like string
        std::string ToJson() const;

        // Serialize to formatted log string
        std::string ToLogString() const;

        // Get all keys
        std::vector<std::string> GetKeys() const;

        // Clear all data
        void Clear();

        // Check if empty
        bool IsEmpty() const { return m_data.empty() && m_nested.empty(); }

    private:
        std::map<std::string, std::string> m_data;
        std::map<std::string, ActionData> m_nested;
        std::map<std::string, std::vector<std::string>> m_arrays;
    };

    /**
     * @brief Memory access information for access violations
     */
    struct MemoryAccessInfo {
        void* faultAddress = nullptr;       // Address that caused the fault
        bool isWrite = false;               // true = write, false = read
        bool isExecute = false;             // DEP violation (execute on non-executable memory)
        std::string addressInfo;            // Additional info about the address
    };

    /**
     * @brief CPU register state (platform-specific)
     */
    struct RegisterState {
        std::map<std::string, std::string> registers;

        void Set(const std::string& name, uint64_t value) {
            std::ostringstream oss;
            oss << "0x" << std::hex << value;
            registers[name] = oss.str();
        }

        std::string ToLogString() const {
            std::ostringstream oss;
            for (const auto& pair : registers) {
                oss << pair.first << "=" << pair.second << " ";
            }
            return oss.str();
        }
    };

    /**
     * @brief Information about a loaded module/DLL
     */
    struct ModuleInfo {
        std::string name;
        std::string path;
        void* baseAddress = nullptr;
        size_t size = 0;
    };

    /**
     * @brief Action history entry for crash context
     */
    struct ActionHistoryEntry {
        std::string action;
        std::string context;
        std::string details;
        std::string timestamp;
        unsigned long sequenceId = 0;
    };

    /**
     * @brief Debug context captured at error/crash time
     */
    struct DebugContext {
        std::vector<StackFrame> stackTrace;
        ActionData variables;               // Captured variable states
        ActionData systemInfo;              // System state at time of error
        std::string errorMessage;
        std::string errorType;              // e.g., "SIGSEGV", "Access Violation", "Exception"
        unsigned long threadId = 0;
        std::string timestamp;

        // Extended crash info
        MemoryAccessInfo memoryAccess;      // For ACCESS_VIOLATION
        RegisterState registers;            // CPU register state
        std::vector<ModuleInfo> loadedModules;  // Loaded DLLs/libraries
        std::vector<ActionHistoryEntry> recentActions;  // Last N actions before crash
        ActionData lastKnownContext;        // Last tracked context variables
    };

    /**
     * @brief Platform-independent stack trace and debug utilities
     */
    class DebugStackTrace {
    public:
        /**
         * @brief Capture current stack trace
         * @param skipFrames Number of frames to skip (to exclude this function itself)
         * @param maxFrames Maximum number of frames to capture
         * @return Vector of stack frames
         */
        static std::vector<StackFrame> Capture(int skipFrames = 1, int maxFrames = 64);

        /**
         * @brief Format stack trace to string
         */
        static std::string FormatStackTrace(const std::vector<StackFrame>& frames);

        /**
         * @brief Get current thread ID
         */
        static unsigned long GetCurrentThreadId();

        /**
         * @brief Initialize crash handlers (call once at startup)
         */
        static void InstallCrashHandlers();

        /**
         * @brief Uninstall crash handlers
         */
        static void UninstallCrashHandlers();

        /**
         * @brief Set callback for crash events
         * @param callback Function to call when crash occurs (before termination)
         */
        static void SetCrashCallback(std::function<void(const DebugContext&)> callback);

        /**
         * @brief Manually trigger a debug dump with current context
         */
        static void DumpDebugContext(const std::string& reason, const ActionData& contextData = ActionData());

        /**
         * @brief Get system information
         */
        static ActionData GetSystemInfo();

        /**
         * @brief Check if running under debugger
         */
        static bool IsDebuggerPresent();

        /**
         * @brief Trigger a breakpoint if debugger is attached
         */
        static void TriggerBreakpoint();

        /**
         * @brief Add action to history buffer (called by EditorActionLogger)
         */
        static void AddToActionHistory(const std::string& action, const std::string& context,
                                       const std::string& details, unsigned long seqId);

        /**
         * @brief Set current context data (for tracking active operations)
         */
        static void SetCurrentContext(const ActionData& context);

        /**
         * @brief Clear current context
         */
        static void ClearCurrentContext();

        /**
         * @brief Get loaded modules list
         */
        static std::vector<ModuleInfo> GetLoadedModules();

        /**
         * @brief Get recent action history
         */
        static std::vector<ActionHistoryEntry> GetRecentActions(int count = 20);

        /**
         * @brief Set maximum action history size
         */
        static void SetActionHistorySize(size_t size);

    private:
        static std::function<void(const DebugContext&)> s_crashCallback;
        static std::vector<ActionHistoryEntry> s_actionHistory;
        static ActionData s_currentContext;
        static size_t s_maxHistorySize;

        // Platform-specific implementations
        static std::vector<StackFrame> CaptureWindows(int skipFrames, int maxFrames);
        static std::vector<StackFrame> CaptureUnix(int skipFrames, int maxFrames);

        static void InstallCrashHandlersWindows();
        static void InstallCrashHandlersUnix();

        // Signal/exception handlers
        static void HandleCrash(const std::string& errorType, const std::string& errorMessage,
                                const MemoryAccessInfo& memAccess = MemoryAccessInfo(),
                                const RegisterState& regs = RegisterState());

#ifdef _WIN32
        static long __stdcall WindowsExceptionHandler(void* exceptionInfo);
        static RegisterState CaptureRegistersWindows(void* ctx);
        static MemoryAccessInfo ParseAccessViolation(void* record);
        static std::vector<ModuleInfo> GetLoadedModulesWindows();
#else
        static void UnixSignalHandler(int signal, siginfo_t* info, void* context);
        static std::vector<ModuleInfo> GetLoadedModulesUnix();
#endif
    };

    /**
     * @brief RAII helper to track scope for debugging
     * Automatically logs entry/exit of a scope
     */
    class DebugScope {
    public:
        DebugScope(const std::string& scopeName, const ActionData& data = ActionData());
        ~DebugScope();

        // Update scope data
        void UpdateData(const ActionData& data);

        // Mark an error occurred in this scope
        void MarkError(const std::string& error);

    private:
        std::string m_scopeName;
        ActionData m_data;
        bool m_hasError = false;
        std::string m_errorMessage;
    };

    // Macros for debug scopes
    #define DEBUG_SCOPE(name) CSEditor::DebugScope _debugScope##__LINE__(name)
    #define DEBUG_SCOPE_DATA(name, data) CSEditor::DebugScope _debugScope##__LINE__(name, data)

    // Macro to capture context on error
    #define DEBUG_CAPTURE_ERROR(msg) \
        CSEditor::DebugStackTrace::DumpDebugContext(msg)

    #define DEBUG_CAPTURE_ERROR_WITH_DATA(msg, data) \
        CSEditor::DebugStackTrace::DumpDebugContext(msg, data)

}
