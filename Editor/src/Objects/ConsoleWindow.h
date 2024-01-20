//
// Created by ounol on 2023-08-26.
//

#pragma once

#include "Base/WindowBase.h"

namespace CSEditor {

    class ConsoleWindow : public WindowBase {
    public:
        ConsoleWindow();

        ~ConsoleWindow() override;

        void SetUI() override;

        ImGuiTextBuffer m_buffer;
        ImGuiTextFilter m_filter;
        ImVector<int> m_lineOffsets;
        bool m_bIsAutoScroll;

        void Clear();

        void AddLog(const char* buffer, int category);

        void Draw(const char* title, bool* p_open = nullptr);
    };

}
