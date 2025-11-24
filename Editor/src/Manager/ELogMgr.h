#pragma once
#include "../../src/Manager/Base/CoreBase.h"

namespace CSEditor {

    class ConsoleWindow;

    class ELogMgr : public CSE::CoreBase {
    public:
        enum class Category {
            INFO_LOG = 0, WARNING_LOG = 1, ERROR_LOG = 2
        };
    public:
        explicit ELogMgr();
        ~ELogMgr() override;

        void Init() override;

        void RegisterWindow(ConsoleWindow* window);

        void AddLog(const char* log, ELogMgr::Category category = Category::INFO_LOG);

        void ClearLog();

    private:
        ConsoleWindow* m_consoleWindow = nullptr;
    };

}
