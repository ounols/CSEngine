//
// Created by ounols on 19. 9. 15.
//

#pragma once

#include "../../SObject.h"
#include "../Matrix.h"
#include "../Quaternion.h"
#include "../../Manager/ResMgr.h"
#include "../../MacroDef.h"
#include "AnimationUtill.h"
#include <vector>
#include <map>
#include <string>


class Animation : public SResource {
public:
    Animation() {
        SetUndestroyable(true);
    }

    Animation(float totalTime, std::vector<KeyFrame*> keyframes) {
        SetUndestroyable(true);
        m_length = totalTime;
        m_keyframes = keyframes;
    }

    ~Animation() {
    }

    void SetKeyframe(float totalTime, std::vector<KeyFrame*> keyframes);

    void Exterminate() override;

    float GetLength() const {
        return m_length;
    }

    std::vector<KeyFrame*> GetKeyFrames() const {
        return m_keyframes;
    }

protected:
    void Init(const AssetMgr::AssetReference* asset) override;
private:
    float m_length = 0;
    std::vector<KeyFrame*> m_keyframes;

};


