//
// Created by ounols on 19. 9. 15.
//

#include "Animation.h"
#include "../Loader/DAE/DAELoader.h"


void Animation::SetKeyframe(float totalTime, std::vector<KeyFrame*> keyframes) {
    m_length = totalTime;
    m_keyframes = keyframes;
}

void Animation::Exterminate() {
    for(auto keyframe : m_keyframes) {
        SAFE_DELETE(keyframe);
    }
    m_keyframes.clear();
}

void Animation::Init(const AssetMgr::AssetReference* asset) {
    std::string parent_id = split(asset->id, '?')[0];
    auto model = ResMgr::getInstance()->GetAssetReference(parent_id);

    DAELoader::GeneratePrefab(model->path.c_str(), nullptr, nullptr, this, nullptr);

}
