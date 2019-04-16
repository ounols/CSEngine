//
// Created by ounols on 19. 3. 31.
//

#pragma once


#include "../Matrix.h"
#include "../XML/XML.h"

struct JointTransformData {
    std::string jointNameId;
    mat4 jointLocalTransform;

    JointTransformData(const std::string& jointNameId, const mat4& jointLocalTransform) : jointNameId(jointNameId),
                                                                                          jointLocalTransform(
                                                                                                  jointLocalTransform) {}
};

struct KeyFrameData {
    float time = 0;
    std::vector<JointTransformData*> jointTransforms;

    KeyFrameData(float time) : time(time) {}
};

struct AnimationData {
    float lengthSeconds;
    std::vector<KeyFrameData*> keyFrames;

    AnimationData(float lengthSeconds, std::vector<KeyFrameData*> keyFrames) : lengthSeconds(lengthSeconds),
                                                                                      keyFrames(keyFrames) {}
};

class DAEAnimationLoader {
public:
    DAEAnimationLoader();
    ~DAEAnimationLoader();

    void Load(const char* path, std::string name);

    AnimationData* GetAnimation() const {
        return m_animationData;
    }

private:
    void LoadAnimation();

    std::string findRootJointName();

    std::vector<float> getKeyTimes();

    std::vector<KeyFrameData*> initKeyFrames(std::vector<float> times);

    void loadJointTransforms(std::vector<KeyFrameData*> frames, XNode jointData, std::string rootNodeId);

    std::string getJointName(XNode jointData);

    std::string getDataId(XNode jointData);

    void
    processTransforms(std::string jointName, std::vector<float> rawData, std::vector<KeyFrameData*> keyFrames, bool root);



private:
    std::string m_name;
    const XNode* m_root;
    XNode m_joint;
    XNode m_animation;

    AnimationData* m_animationData = nullptr;
};


