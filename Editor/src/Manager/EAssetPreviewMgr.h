#pragma once

#include "../../src/Manager/Base/CoreBase.h"
#include <unordered_map>
#include <string>

namespace CSE {
    class SResource;
    class STexture;
    class SFrameBuffer;
    class SMaterial;
    class ResMgr;
}

namespace CSEditor {
    class EAssetPreviewMgr : public CSE::CoreBase {
    public:
        EAssetPreviewMgr();

        ~EAssetPreviewMgr() override;

        void Init() override;

        CSE::STexture* GetPreview(std::string& hash);

        CSE::SResource* GeneratePreview(std::string& hash);

    private:
        CSE::SFrameBuffer* GenerateFrameBuffer(const CSE::SMaterial& material) const;

    private:
        std::unordered_map<std::string, CSE::SResource*> m_previews;
        CSE::ResMgr* m_editorResMgr = nullptr;
    };
}