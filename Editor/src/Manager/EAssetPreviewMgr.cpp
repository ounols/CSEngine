#include "EAssetPreviewMgr.h"

#include "EEngineCore.h"
#include "../../src/Util/Render/SFrameBuffer.h"
#include "../../src/Manager/ResMgr.h"
#include "../../src/Util/Render/STexture.h"

using namespace CSEditor;
using namespace CSE;

EAssetPreviewMgr::EAssetPreviewMgr() {

}

EAssetPreviewMgr::~EAssetPreviewMgr() {

}

void EAssetPreviewMgr::Init() {
    m_editorResMgr = EEngineCore::getEditorInstance()->GetResMgrCore();
}

CSE::STexture* EAssetPreviewMgr::GetPreview(std::string& hash) {
    const auto& iter = m_previews.find(hash);
    if (iter != m_previews.end()) return static_cast<STexture*>(iter->second);

    return static_cast<STexture*>(GeneratePreview(hash));
}

SResource* EAssetPreviewMgr::GeneratePreview(std::string& hash) {
    const auto& asset = m_editorResMgr->GetAssetReference(hash);
    const auto& res = SResource::Create(asset, asset->class_type);

    if (res->IsSameClass(STexture::GetClassStaticType())) {
        m_previews.insert(std::pair<std::string, CSE::SResource*>(res->GetHash(), res));
        return res;
    }
}

CSE::SFrameBuffer* EAssetPreviewMgr::GenerateFrameBuffer(const SMaterial& material) const {
    return nullptr;
}