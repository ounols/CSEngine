#pragma once

#include "Base/WindowBase.h"

namespace CSEditor {

    class EEngineCore;

    class PreviewWindow : public WindowBase {
    public:
        PreviewWindow();

        ~PreviewWindow() override;

        void SetUI() override;

        void InitPreview();

        void ReleasePreview();

        bool IsPreview() const;
    private:
        EEngineCore* m_engineCore = nullptr;
        bool m_bIsInit = false;

        unsigned int m_prevWidth;
        unsigned int m_prevHeight;
    };
}