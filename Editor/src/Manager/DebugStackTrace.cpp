#include "DebugStackTrace.h"
#include "EditorActionLogger.h"
#include <thread>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#include <Psapi.h>   // For EnumProcessModules
#include <io.h>      // For _commit, _fileno
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Psapi.lib")
#else
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <link.h>    // For dl_iterate_phdr
#endif

#include <mutex>
#include <chrono>

using namespace CSEditor;

// Static member initialization
std::function<void(const DebugContext&)> DebugStackTrace::s_crashCallback = nullptr;
std::vector<ActionHistoryEntry> DebugStackTrace::s_actionHistory;
ActionData DebugStackTrace::s_currentContext;
size_t DebugStackTrace::s_maxHistorySize = 50;
static std::mutex s_historyMutex;

// ========== ActionData Implementation ==========

ActionData& ActionData::Set(const std::string& key, const std::string& value) {
    m_data[key] = value;
    return *this;
}

ActionData& ActionData::Set(const std::string& key, int value) {
    m_data[key] = std::to_string(value);
    return *this;
}

ActionData& ActionData::Set(const std::string& key, float value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    m_data[key] = oss.str();
    return *this;
}

ActionData& ActionData::Set(const std::string& key, double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(10) << value;
    m_data[key] = oss.str();
    return *this;
}

ActionData& ActionData::Set(const std::string& key, bool value) {
    m_data[key] = value ? "true" : "false";
    return *this;
}

ActionData& ActionData::Set(const std::string& key, const char* value) {
    m_data[key] = value ? value : "null";
    return *this;
}

ActionData& ActionData::SetPtr(const std::string& key, const void* ptr) {
    std::ostringstream oss;
    oss << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
    m_data[key] = oss.str();
    return *this;
}

ActionData& ActionData::SetNested(const std::string& key, const ActionData& nested) {
    m_nested[key] = nested;
    return *this;
}

ActionData& ActionData::SetArray(const std::string& key, const std::vector<std::string>& values) {
    m_arrays[key] = values;
    return *this;
}

std::string ActionData::Get(const std::string& key) const {
    auto it = m_data.find(key);
    return (it != m_data.end()) ? it->second : "";
}

bool ActionData::Has(const std::string& key) const {
    return m_data.find(key) != m_data.end() ||
           m_nested.find(key) != m_nested.end() ||
           m_arrays.find(key) != m_arrays.end();
}

std::string ActionData::ToJson() const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;

    // Simple key-value pairs
    for (const auto& pair : m_data) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << pair.first << "\":\"" << pair.second << "\"";
    }

    // Nested objects
    for (const auto& pair : m_nested) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << pair.first << "\":" << pair.second.ToJson();
    }

    // Arrays
    for (const auto& pair : m_arrays) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << pair.first << "\":[";
        bool arrFirst = true;
        for (const auto& val : pair.second) {
            if (!arrFirst) oss << ",";
            arrFirst = false;
            oss << "\"" << val << "\"";
        }
        oss << "]";
    }

    oss << "}";
    return oss.str();
}

std::string ActionData::ToLogString() const {
    std::ostringstream oss;
    bool first = true;

    for (const auto& pair : m_data) {
        if (!first) oss << ", ";
        first = false;
        oss << pair.first << "=" << pair.second;
    }

    for (const auto& pair : m_nested) {
        if (!first) oss << ", ";
        first = false;
        oss << pair.first << "={" << pair.second.ToLogString() << "}";
    }

    for (const auto& pair : m_arrays) {
        if (!first) oss << ", ";
        first = false;
        oss << pair.first << "=[";
        bool arrFirst = true;
        for (const auto& val : pair.second) {
            if (!arrFirst) oss << ",";
            arrFirst = false;
            oss << val;
        }
        oss << "]";
    }

    return oss.str();
}

std::vector<std::string> ActionData::GetKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : m_data) keys.push_back(pair.first);
    for (const auto& pair : m_nested) keys.push_back(pair.first);
    for (const auto& pair : m_arrays) keys.push_back(pair.first);
    return keys;
}

void ActionData::Clear() {
    m_data.clear();
    m_nested.clear();
    m_arrays.clear();
}

// ========== DebugStackTrace Implementation ==========

std::vector<StackFrame> DebugStackTrace::Capture(int skipFrames, int maxFrames) {
#ifdef _WIN32
    return CaptureWindows(skipFrames + 1, maxFrames);
#else
    return CaptureUnix(skipFrames + 1, maxFrames);
#endif
}

#ifdef _WIN32
std::vector<StackFrame> DebugStackTrace::CaptureWindows(int skipFrames, int maxFrames) {
    std::vector<StackFrame> frames;

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
    SymInitialize(process, NULL, TRUE);

    CONTEXT context;
    RtlCaptureContext(&context);

    STACKFRAME64 stackFrame = {};
    DWORD machineType;

#ifdef _M_X64
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
    machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#endif

    int frameCount = 0;
    int skipped = 0;

    while (StackWalk64(machineType, process, thread, &stackFrame, &context,
                       NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
        if (stackFrame.AddrPC.Offset == 0) break;

        if (skipped < skipFrames) {
            skipped++;
            continue;
        }

        if (frameCount >= maxFrames) break;

        StackFrame frame;
        frame.address = reinterpret_cast<void*>(stackFrame.AddrPC.Offset);

        // Get function name
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        DWORD64 displacement = 0;
        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol)) {
            frame.function = symbol->Name;
        } else {
            std::ostringstream oss;
            oss << "0x" << std::hex << stackFrame.AddrPC.Offset;
            frame.function = oss.str();
        }

        // Get line info
        IMAGEHLP_LINE64 lineInfo = {};
        lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD lineDisplacement = 0;
        if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &lineInfo)) {
            frame.file = lineInfo.FileName;
            frame.line = lineInfo.LineNumber;
        }

        // Get module name
        IMAGEHLP_MODULE64 moduleInfo = {};
        moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
        if (SymGetModuleInfo64(process, stackFrame.AddrPC.Offset, &moduleInfo)) {
            frame.module = moduleInfo.ModuleName;
        }

        frames.push_back(frame);
        frameCount++;
    }

    SymCleanup(process);
    return frames;
}
#else
std::vector<StackFrame> DebugStackTrace::CaptureUnix(int skipFrames, int maxFrames) {
    std::vector<StackFrame> frames;

    void* buffer[128];
    int numFrames = backtrace(buffer, std::min(128, maxFrames + skipFrames));
    char** symbols = backtrace_symbols(buffer, numFrames);

    if (symbols == nullptr) return frames;

    for (int i = skipFrames; i < numFrames && (i - skipFrames) < maxFrames; i++) {
        StackFrame frame;
        frame.address = buffer[i];

        // Parse symbol string
        std::string symbolStr = symbols[i];
        frame.function = symbolStr;

        // Try to demangle C++ names
        Dl_info info;
        if (dladdr(buffer[i], &info) && info.dli_sname) {
            int status = 0;
            char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            if (status == 0 && demangled) {
                frame.function = demangled;
                free(demangled);
            } else if (info.dli_sname) {
                frame.function = info.dli_sname;
            }

            if (info.dli_fname) {
                frame.module = info.dli_fname;
            }
        }

        frames.push_back(frame);
    }

    free(symbols);
    return frames;
}
#endif

std::string DebugStackTrace::FormatStackTrace(const std::vector<StackFrame>& frames) {
    std::ostringstream oss;
    oss << "Stack Trace (" << frames.size() << " frames):\n";

    for (size_t i = 0; i < frames.size(); i++) {
        const auto& frame = frames[i];
        oss << "  [" << i << "] " << frame.function;

        if (!frame.file.empty()) {
            oss << " at " << frame.file;
            if (frame.line > 0) {
                oss << ":" << frame.line;
            }
        }

        if (!frame.module.empty()) {
            oss << " (" << frame.module << ")";
        }

        oss << "\n";
    }

    return oss.str();
}

unsigned long DebugStackTrace::GetCurrentThreadId() {
#ifdef _WIN32
    return static_cast<unsigned long>(::GetCurrentThreadId());
#else
    return static_cast<unsigned long>(pthread_self());
#endif
}

void DebugStackTrace::InstallCrashHandlers() {
#ifdef _WIN32
    InstallCrashHandlersWindows();
#else
    InstallCrashHandlersUnix();
#endif
}

#ifdef _WIN32
void DebugStackTrace::InstallCrashHandlersWindows() {
    SetUnhandledExceptionFilter(reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(WindowsExceptionHandler));
}

MemoryAccessInfo DebugStackTrace::ParseAccessViolation(void* recordPtr) {
    MemoryAccessInfo info;
    auto* record = static_cast<EXCEPTION_RECORD*>(recordPtr);

    if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
        // First parameter: 0 = read, 1 = write, 8 = DEP (execute)
        ULONG_PTR accessType = record->ExceptionInformation[0];
        info.faultAddress = reinterpret_cast<void*>(record->ExceptionInformation[1]);
        info.isWrite = (accessType == 1);
        info.isExecute = (accessType == 8);

        std::ostringstream oss;
        oss << "0x" << std::hex << record->ExceptionInformation[1];

        if (accessType == 0) {
            info.addressInfo = "READ at " + oss.str();
        } else if (accessType == 1) {
            info.addressInfo = "WRITE at " + oss.str();
        } else if (accessType == 8) {
            info.addressInfo = "DEP violation (execute) at " + oss.str();
        } else {
            info.addressInfo = "Unknown access at " + oss.str();
        }

        // Check if it's likely a null pointer dereference
        if (record->ExceptionInformation[1] < 0x10000) {
            info.addressInfo += " [likely NULL pointer dereference]";
        }
    }

    return info;
}

RegisterState DebugStackTrace::CaptureRegistersWindows(void* ctxPtr) {
    RegisterState regs;
    auto* ctx = static_cast<CONTEXT*>(ctxPtr);

#ifdef _M_X64
    regs.Set("RAX", ctx->Rax);
    regs.Set("RBX", ctx->Rbx);
    regs.Set("RCX", ctx->Rcx);
    regs.Set("RDX", ctx->Rdx);
    regs.Set("RSI", ctx->Rsi);
    regs.Set("RDI", ctx->Rdi);
    regs.Set("RBP", ctx->Rbp);
    regs.Set("RSP", ctx->Rsp);
    regs.Set("R8", ctx->R8);
    regs.Set("R9", ctx->R9);
    regs.Set("R10", ctx->R10);
    regs.Set("R11", ctx->R11);
    regs.Set("R12", ctx->R12);
    regs.Set("R13", ctx->R13);
    regs.Set("R14", ctx->R14);
    regs.Set("R15", ctx->R15);
    regs.Set("RIP", ctx->Rip);
    regs.Set("EFLAGS", ctx->EFlags);
#else
    regs.Set("EAX", ctx->Eax);
    regs.Set("EBX", ctx->Ebx);
    regs.Set("ECX", ctx->Ecx);
    regs.Set("EDX", ctx->Edx);
    regs.Set("ESI", ctx->Esi);
    regs.Set("EDI", ctx->Edi);
    regs.Set("EBP", ctx->Ebp);
    regs.Set("ESP", ctx->Esp);
    regs.Set("EIP", ctx->Eip);
    regs.Set("EFLAGS", ctx->EFlags);
#endif

    return regs;
}

std::vector<ModuleInfo> DebugStackTrace::GetLoadedModulesWindows() {
    std::vector<ModuleInfo> modules;

    HANDLE process = ::GetCurrentProcess();
    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(process, hMods, sizeof(hMods), &cbNeeded)) {
        int count = cbNeeded / sizeof(HMODULE);
        for (int i = 0; i < count && i < 100; i++) {  // Limit to 100 modules
            ModuleInfo mod;
            mod.baseAddress = hMods[i];

            char modName[MAX_PATH];
            if (GetModuleFileNameExA(process, hMods[i], modName, sizeof(modName))) {
                mod.path = modName;
                // Extract just the filename
                const char* lastSlash = strrchr(modName, '\\');
                mod.name = lastSlash ? (lastSlash + 1) : modName;
            }

            MODULEINFO modInfo;
            if (GetModuleInformation(process, hMods[i], &modInfo, sizeof(modInfo))) {
                mod.size = modInfo.SizeOfImage;
            }

            modules.push_back(mod);
        }
    }

    return modules;
}

long __stdcall DebugStackTrace::WindowsExceptionHandler(void* exceptionInfoPtr) {
    auto* exceptionInfo = static_cast<EXCEPTION_POINTERS*>(exceptionInfoPtr);
    std::string errorType;
    std::string errorMessage;
    MemoryAccessInfo memAccess;
    RegisterState regs;

    // Capture registers first
    if (exceptionInfo->ContextRecord) {
        regs = CaptureRegistersWindows(exceptionInfo->ContextRecord);
    }

    switch (exceptionInfo->ExceptionRecord->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            errorType = "ACCESS_VIOLATION";
            memAccess = ParseAccessViolation(exceptionInfo->ExceptionRecord);
            errorMessage = "Memory access violation - " + memAccess.addressInfo;
            break;
        case EXCEPTION_STACK_OVERFLOW:
            errorType = "STACK_OVERFLOW";
            errorMessage = "Stack overflow";
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            errorType = "DIVIDE_BY_ZERO";
            errorMessage = "Integer divide by zero";
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            errorType = "FLT_DIVIDE_BY_ZERO";
            errorMessage = "Floating point divide by zero";
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            errorType = "ILLEGAL_INSTRUCTION";
            errorMessage = "Illegal instruction";
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            errorType = "ARRAY_BOUNDS_EXCEEDED";
            errorMessage = "Array bounds exceeded";
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            errorType = "DATATYPE_MISALIGNMENT";
            errorMessage = "Data type misalignment";
            break;
        case EXCEPTION_IN_PAGE_ERROR:
            errorType = "IN_PAGE_ERROR";
            errorMessage = "Page fault - memory could not be accessed";
            break;
        default:
            errorType = "UNKNOWN_EXCEPTION";
            std::ostringstream oss;
            oss << "Exception code: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionCode;
            errorMessage = oss.str();
            break;
    }

    HandleCrash(errorType, errorMessage, memAccess, regs);
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
void DebugStackTrace::InstallCrashHandlersUnix() {
    struct sigaction sa;
    sa.sa_sigaction = UnixSignalHandler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGFPE, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGBUS, &sa, nullptr);
}

// Callback for dl_iterate_phdr to enumerate loaded modules
static int dl_iterate_callback(struct dl_phdr_info* info, size_t size, void* data) {
    auto* modules = static_cast<std::vector<ModuleInfo>*>(data);
    if (modules->size() >= 100) return 1;  // Limit

    ModuleInfo mod;
    mod.path = info->dlpi_name ? info->dlpi_name : "";
    mod.baseAddress = reinterpret_cast<void*>(info->dlpi_addr);

    // Extract just the filename
    if (!mod.path.empty()) {
        size_t lastSlash = mod.path.rfind('/');
        mod.name = (lastSlash != std::string::npos) ? mod.path.substr(lastSlash + 1) : mod.path;
    } else {
        mod.name = "[main]";
    }

    // Calculate total size from program headers
    for (int i = 0; i < info->dlpi_phnum; i++) {
        if (info->dlpi_phdr[i].p_type == PT_LOAD) {
            size_t end = info->dlpi_phdr[i].p_vaddr + info->dlpi_phdr[i].p_memsz;
            if (end > mod.size) mod.size = end;
        }
    }

    modules->push_back(mod);
    return 0;
}

std::vector<ModuleInfo> DebugStackTrace::GetLoadedModulesUnix() {
    std::vector<ModuleInfo> modules;
    dl_iterate_phdr(dl_iterate_callback, &modules);
    return modules;
}

void DebugStackTrace::UnixSignalHandler(int sig, siginfo_t* info, void* context) {
    std::string errorType;
    std::string errorMessage;
    MemoryAccessInfo memAccess;

    switch (sig) {
        case SIGSEGV:
            errorType = "SIGSEGV";
            errorMessage = "Segmentation fault";
            if (info && info->si_addr) {
                memAccess.faultAddress = info->si_addr;
                std::ostringstream oss;
                oss << "0x" << std::hex << reinterpret_cast<uintptr_t>(info->si_addr);
                memAccess.addressInfo = "Fault at " + oss.str();
                if (reinterpret_cast<uintptr_t>(info->si_addr) < 0x10000) {
                    memAccess.addressInfo += " [likely NULL pointer dereference]";
                }
            }
            break;
        case SIGABRT:
            errorType = "SIGABRT";
            errorMessage = "Abort signal";
            break;
        case SIGFPE:
            errorType = "SIGFPE";
            errorMessage = "Floating point exception";
            break;
        case SIGILL:
            errorType = "SIGILL";
            errorMessage = "Illegal instruction";
            break;
        case SIGBUS:
            errorType = "SIGBUS";
            errorMessage = "Bus error";
            if (info && info->si_addr) {
                memAccess.faultAddress = info->si_addr;
            }
            break;
        default:
            errorType = "UNKNOWN_SIGNAL";
            errorMessage = "Signal " + std::to_string(sig);
            break;
    }

    if (!memAccess.addressInfo.empty()) {
        errorMessage += " - " + memAccess.addressInfo;
    }

    HandleCrash(errorType, errorMessage, memAccess, RegisterState());

    // Re-raise signal for default handler
    signal(sig, SIG_DFL);
    raise(sig);
}
#endif

void DebugStackTrace::UninstallCrashHandlers() {
#ifdef _WIN32
    SetUnhandledExceptionFilter(nullptr);
#else
    signal(SIGSEGV, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
#endif
}

void DebugStackTrace::SetCrashCallback(std::function<void(const DebugContext&)> callback) {
    s_crashCallback = callback;
}

// ========== Action History Management ==========

void DebugStackTrace::AddToActionHistory(const std::string& action, const std::string& context,
                                          const std::string& details, unsigned long seqId) {
    std::lock_guard<std::mutex> lock(s_historyMutex);

    ActionHistoryEntry entry;
    entry.action = action;
    entry.context = context;
    entry.details = details;
    entry.sequenceId = seqId;

    // Get timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    entry.timestamp = oss.str();

    s_actionHistory.push_back(entry);

    // Trim if too large
    while (s_actionHistory.size() > s_maxHistorySize) {
        s_actionHistory.erase(s_actionHistory.begin());
    }
}

void DebugStackTrace::SetCurrentContext(const ActionData& context) {
    s_currentContext = context;
}

void DebugStackTrace::ClearCurrentContext() {
    s_currentContext.Clear();
}

std::vector<ActionHistoryEntry> DebugStackTrace::GetRecentActions(int count) {
    std::lock_guard<std::mutex> lock(s_historyMutex);

    std::vector<ActionHistoryEntry> result;
    int start = max(0, static_cast<int>(s_actionHistory.size()) - count);
    for (size_t i = start; i < s_actionHistory.size(); i++) {
        result.push_back(s_actionHistory[i]);
    }
    return result;
}

void DebugStackTrace::SetActionHistorySize(size_t size) {
    s_maxHistorySize = size;
}

std::vector<ModuleInfo> DebugStackTrace::GetLoadedModules() {
#ifdef _WIN32
    return GetLoadedModulesWindows();
#else
    return GetLoadedModulesUnix();
#endif
}

// ========== Crash Handling ==========

void DebugStackTrace::HandleCrash(const std::string& errorType, const std::string& errorMessage,
                                   const MemoryAccessInfo& memAccess, const RegisterState& regs) {
    DebugContext ctx;
    ctx.errorType = errorType;
    ctx.errorMessage = errorMessage;
    ctx.stackTrace = Capture(2, 64);
    ctx.threadId = GetCurrentThreadId();
    ctx.systemInfo = GetSystemInfo();
    ctx.memoryAccess = memAccess;
    ctx.registers = regs;
    ctx.lastKnownContext = s_currentContext;

    // Get recent actions (without locking, as we might be in crash state)
    ctx.recentActions = s_actionHistory;

    // Get loaded modules
    ctx.loadedModules = GetLoadedModules();

    // Get timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    ctx.timestamp = oss.str();

    // Build comprehensive crash log
    std::string crashLog = "\n";
    crashLog += "╔══════════════════════════════════════════════════════════════════╗\n";
    crashLog += "║                        CRASH REPORT                              ║\n";
    crashLog += "╚══════════════════════════════════════════════════════════════════╝\n\n";

    crashLog += ">>> BASIC INFO\n";
    crashLog += "Time: " + ctx.timestamp + "\n";
    crashLog += "Error Type: " + ctx.errorType + "\n";
    crashLog += "Message: " + ctx.errorMessage + "\n";
    crashLog += "Thread ID: " + std::to_string(ctx.threadId) + "\n\n";

    // Memory access info for access violations
    if (memAccess.faultAddress != nullptr || !memAccess.addressInfo.empty()) {
        crashLog += ">>> MEMORY ACCESS DETAILS\n";
        if (memAccess.faultAddress) {
            std::ostringstream addrOss;
            addrOss << "0x" << std::hex << reinterpret_cast<uintptr_t>(memAccess.faultAddress);
            crashLog += "Fault Address: " + addrOss.str() + "\n";
        }
        crashLog += "Access Type: " + std::string(memAccess.isWrite ? "WRITE" : (memAccess.isExecute ? "EXECUTE" : "READ")) + "\n";
        crashLog += "Details: " + memAccess.addressInfo + "\n\n";
    }

    // Register state
    if (!regs.registers.empty()) {
        crashLog += ">>> CPU REGISTERS\n";
        crashLog += regs.ToLogString() + "\n\n";
    }

    // Stack trace
    crashLog += ">>> STACK TRACE\n";
    crashLog += FormatStackTrace(ctx.stackTrace) + "\n";

    // Recent actions (what the user was doing before crash)
    if (!ctx.recentActions.empty()) {
        crashLog += ">>> RECENT ACTIONS (last " + std::to_string(ctx.recentActions.size()) + " before crash)\n";
        for (const auto& action : ctx.recentActions) {
            crashLog += "  [" + std::to_string(action.sequenceId) + "] " + action.timestamp + " - ";
            crashLog += action.action;
            if (!action.context.empty()) {
                crashLog += " (" + action.context + ")";
            }
            crashLog += "\n";
        }
        crashLog += "\n";
    }

    // Last known context
    if (!ctx.lastKnownContext.IsEmpty()) {
        crashLog += ">>> LAST KNOWN CONTEXT\n";
        crashLog += ctx.lastKnownContext.ToLogString() + "\n\n";
    }

    // System info
    crashLog += ">>> SYSTEM INFO\n";
    crashLog += ctx.systemInfo.ToLogString() + "\n\n";

    // Loaded modules (first 20)
    if (!ctx.loadedModules.empty()) {
        crashLog += ">>> LOADED MODULES (showing first 20)\n";
        int count = 0;
        for (const auto& mod : ctx.loadedModules) {
            if (count++ >= 20) break;
            std::ostringstream modOss;
            modOss << "  " << mod.name << " @ 0x" << std::hex << reinterpret_cast<uintptr_t>(mod.baseAddress);
            modOss << " (size: " << std::dec << (mod.size / 1024) << " KB)";
            crashLog += modOss.str() + "\n";
        }
        crashLog += "\n";
    }

    crashLog += "═══════════════════════════════════════════════════════════════════\n";

    // Write to stderr immediately and flush
    fprintf(stderr, "%s", crashLog.c_str());
    fflush(stderr);

    // Try to save to file using C-style I/O (safer during crashes)
    const char* crashPaths[] = {
        "crash_report.log",
        "./crash_report.log",
#ifdef _WIN32
        "C:\\Temp\\csengine_crash.log",
#else
        "/tmp/csengine_crash.log",
#endif
        nullptr
    };

    FILE* crashFile = nullptr;
    for (int i = 0; crashPaths[i] != nullptr; i++) {
        crashFile = fopen(crashPaths[i], "a");
        if (crashFile) {
            fprintf(crashFile, "%s\n", crashLog.c_str());
            fflush(crashFile);
#ifdef _WIN32
            _commit(_fileno(crashFile));
#else
            fsync(fileno(crashFile));
#endif
            fclose(crashFile);
            fprintf(stderr, "[Crash report saved to: %s]\n", crashPaths[i]);
            fflush(stderr);
            break;
        }
    }

    if (!crashFile) {
        fprintf(stderr, "[WARNING: Could not save crash report to file]\n");
        fflush(stderr);
    }

    // Call callback if set
    if (s_crashCallback) {
        s_crashCallback(ctx);
    }
}

void DebugStackTrace::DumpDebugContext(const std::string& reason, const ActionData& contextData) {
    DebugContext ctx;
    ctx.errorType = "DEBUG_DUMP";
    ctx.errorMessage = reason;
    ctx.stackTrace = Capture(2, 32);
    ctx.threadId = GetCurrentThreadId();
    ctx.variables = contextData;

    std::string dumpLog = "\n----- Debug Context Dump -----\n";
    dumpLog += "Reason: " + reason + "\n";
    if (!contextData.IsEmpty()) {
        dumpLog += "Context: " + contextData.ToLogString() + "\n";
    }
    dumpLog += FormatStackTrace(ctx.stackTrace);
    dumpLog += "------------------------------\n";

    // Log through EditorActionLogger
    ACTION_LOG_SYSTEM(ActionSeverity::DEBUG, "Debug dump", dumpLog);
}

ActionData DebugStackTrace::GetSystemInfo() {
    ActionData info;

#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        info.Set("total_physical_memory_mb", static_cast<int>(memInfo.ullTotalPhys / (1024 * 1024)));
        info.Set("available_physical_memory_mb", static_cast<int>(memInfo.ullAvailPhys / (1024 * 1024)));
        info.Set("memory_load_percent", static_cast<int>(memInfo.dwMemoryLoad));
    }

    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);  // Use global namespace to call Windows API
    info.Set("processor_count", static_cast<int>(sysInfo.dwNumberOfProcessors));
    info.Set("platform", "Windows");
#else
    info.Set("platform", "Unix/Linux");

    // Try to get memory info from /proc/meminfo
    FILE* meminfo = fopen("/proc/meminfo", "r");
    if (meminfo) {
        char line[256];
        while (fgets(line, sizeof(line), meminfo)) {
            unsigned long value;
            if (sscanf(line, "MemTotal: %lu kB", &value) == 1) {
                info.Set("total_physical_memory_mb", static_cast<int>(value / 1024));
            }
            if (sscanf(line, "MemAvailable: %lu kB", &value) == 1) {
                info.Set("available_physical_memory_mb", static_cast<int>(value / 1024));
            }
        }
        fclose(meminfo);
    }
#endif

    return info;
}

bool DebugStackTrace::IsDebuggerPresent() {
#ifdef _WIN32
    return ::IsDebuggerPresent() != 0;
#else
    // Check /proc/self/status for TracerPid
    FILE* status = fopen("/proc/self/status", "r");
    if (status) {
        char line[256];
        while (fgets(line, sizeof(line), status)) {
            int tracerPid;
            if (sscanf(line, "TracerPid: %d", &tracerPid) == 1) {
                fclose(status);
                return tracerPid != 0;
            }
        }
        fclose(status);
    }
    return false;
#endif
}

void DebugStackTrace::TriggerBreakpoint() {
    if (IsDebuggerPresent()) {
#ifdef _WIN32
        __debugbreak();
#else
        raise(SIGTRAP);
#endif
    }
}

// ========== DebugScope Implementation ==========

DebugScope::DebugScope(const std::string& scopeName, const ActionData& data)
    : m_scopeName(scopeName), m_data(data) {
    ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::DEBUG,
               "Entering scope: " + scopeName,
               data.IsEmpty() ? "" : data.ToLogString(), "");
}

DebugScope::~DebugScope() {
    if (m_hasError) {
        ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::ERR,
                   "Exiting scope with error: " + m_scopeName,
                   m_errorMessage, "");
    } else {
        ACTION_LOG(ActionCategory::SYSTEM, ActionSeverity::DEBUG,
                   "Exiting scope: " + m_scopeName, "", "");
    }
}

void DebugScope::UpdateData(const ActionData& data) {
    m_data = data;
}

void DebugScope::MarkError(const std::string& error) {
    m_hasError = true;
    m_errorMessage = error;
}
