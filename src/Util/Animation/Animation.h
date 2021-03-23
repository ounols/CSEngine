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
#include <map>
#include <string>
#include <list>

namespace CSE {

    class Animation : public SResource {
    public:
        Animation() {
            SetUndestroyable(true);
        }

        Animation(float totalTime, std::list<KeyFrame*> keyframes) {
            SetUndestroyable(true);
            m_length = totalTime;
            m_keyframes = keyframes;
        }

        ~Animation() {
        }

        void SetKeyframe(float totalTime, std::list<KeyFrame*> keyframes);

        void Exterminate() override;

        float GetLength() const {
            return m_length;
        }

        std::list<KeyFrame*> GetKeyFrames() const {
            return m_keyframes;
        }

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        float m_length = 0;
        std::list<KeyFrame*> m_keyframes;

    };

}