#pragma once

#include "../../../src/Manager/EngineCoreInstance.h"

namespace CSEditor {

    class EPreviewCore;

    class EEngineCore : public CSE::EngineCoreInstance {
    protected:
        DECLARE_SINGLETON(EEngineCore);
        ~EEngineCore() override;

    public:
        void StartPreviewCore(unsigned int width, unsigned int height);
        void StopPreviewCore();
        void UpdatePreviewCore(float elapsedTime);
        void RenderPreviewCore() const;
        bool IsPreview() const {
            return m_previewCore != nullptr;
        }
        EPreviewCore* GetPreviewCore() const {
            return m_previewCore;
        }

    private:
        EPreviewCore* m_previewCore = nullptr;
    };

}
