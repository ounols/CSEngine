//
// Created by ounols on 19. 9. 15.
//

#include "Animation.h"
#include "../Loader/DAE/DAELoader.h"
#include "../../Manager/EngineCore.h"

using namespace CSE;

void Animation::SetKeyframe(float totalTime, std::list<KeyFrame*> keyframes) {
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
    auto model = CORE->GetCore(ResMgr)->GetAssetReference(parent_id);

    // 프리팹 객체에서만 모든걸 만들어야 로직이 꼬이지 않기 때문에 해당 예외처리는 허용되지 않음 (삭제 예정)
//    DAELoader::GeneratePrefab(model->path.c_str(), nullptr, nullptr, this, nullptr);

}
