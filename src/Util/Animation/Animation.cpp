//
// Created by ounols on 19. 9. 15.
//
#ifndef CSE_GLOBAL_SKINNED_ANIMATION_DISABLED
#include "Animation.h"
#include "../Loader/DAE/DAELoader.h"
#include "../../Manager/EngineCore.h"
#include "../MoreString.h"

using namespace CSE;

RESOURCE_CONSTRUCTOR(Animation) {
    SetUndestroyable(true);
}

void Animation::SetKeyframe(float totalTime, std::list<KeyFrame*> keyframes) {
    m_length = totalTime;
    m_keyframes = std::move(keyframes);
}

void Animation::Exterminate() {
    for(auto keyframe : m_keyframes) {
        SAFE_DELETE(keyframe);
    }
    m_keyframes.clear();
}

void Animation::Init(const AssetMgr::AssetReference* asset) {
    std::string parent_id = split(asset->id, '?')[0];
    CORE->GetCore(ResMgr)->GetAssetReference(parent_id);
}

void Animation::SetValue(const std::string& name_str, const Arguments& value) {
}

std::string Animation::PrintValue() const {
    return {};
}
#endif