#pragma once

#include <string>
#include "Base/WindowBase.h"

class XNode;

namespace CSEditor {

    class EEngineCore;

    class InspectorWindow : public WindowBase {
    public:
        InspectorWindow();

        ~InspectorWindow() override;

        void SetUI() override;

    private:
        void GenerateValueInputs(const XNode& buffer);

    private:
        EEngineCore* m_core = nullptr;
    };
}