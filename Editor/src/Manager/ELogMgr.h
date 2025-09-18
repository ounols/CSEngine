#pragma once
#include "../../src/Manager/Base/CoreBase.h"

namespace CSEditor {

    class ConsoleWindow;

    class ELogMgr : public CSE::CoreBase {
    public:
        enum class Category {
            INFO = 0, WARNING = 1, ERROR = 2
        };
    public:
        explicit ELogMgr();
        ~ELogMgr() override;

        void Init() override;

        void RegisterWindow(ConsoleWindow* window);

        void AddLog(const char* log, ELogMgr::Category category = Category::INFO);

        void ClearLog();

    private:
        ConsoleWindow* m_consoleWindow = nullptr;
    };

}
