#include "ELogMgr.h"
#include "../Objects/ConsoleWindow.h"

using namespace CSEditor;

ELogMgr::ELogMgr() = default;

ELogMgr::~ELogMgr() = default;

void ELogMgr::Init() {}

void ELogMgr::RegisterWindow(ConsoleWindow* window) {
    m_consoleWindow = window;
}

void ELogMgr::AddLog(const char* log, Category category) {
    m_consoleWindow->AddLog(log, static_cast<int>(category));
}

void ELogMgr::ClearLog() {
    m_consoleWindow->Clear();
}