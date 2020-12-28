//
// Created by ounols on 19. 3. 31.
//

#include "../../../MacroDef.h"
#include "DAEAnimationLoader.h"

#include "../../MoreString.h"
#include "../../../Manager/ResMgr.h"

using namespace CSE;

const mat4 CORRECTION = /*mat4::RotateX(90)*/ mat4::Identity();


DAEAnimationLoader::DAEAnimationLoader() {

}

DAEAnimationLoader::~DAEAnimationLoader() {
    SAFE_DELETE(m_root);

    for (auto frame : m_animationData->keyFrames) {
        for (auto joint : frame->jointTransforms) {
            SAFE_DELETE(joint);
        }
        frame->jointTransforms.clear();
        SAFE_DELETE(frame);
    }
    m_animationData->keyFrames.clear();
    SAFE_DELETE(m_animationData);
}

void DAEAnimationLoader::Load(const char* path, std::string name) {
    m_name = name;

    m_root = XFILE(path).getRoot();
    XNode collada = m_root->getChild("COLLADA");

    m_animation = collada.getChild("library_animations");
    m_joint = collada.getChild("library_visual_scenes");
    LoadAnimation();
}

void DAEAnimationLoader::LoadAnimation() {
    std::string rootNode = findRootJointName();
    std::vector<float> times = getKeyTimes();
    float duration = times[times.size() - 1];
    std::vector<KeyFrameData*> keyFrames = initKeyFrames(times);
    std::vector<XNode> animationNodes = m_animation.children;
    for (XNode jointNode : animationNodes) {
        if (jointNode.name != "animation") continue;
        loadJointTransforms(keyFrames, jointNode, rootNode);
    }

    m_animationData = new AnimationData(duration, keyFrames);
}

std::string DAEAnimationLoader::findRootJointName() {
    XNode skeleton = m_joint.getChild("visual_scene").getNodeByAttribute("node", "id", "Armature");
    return skeleton.getChild("node").getAttribute("id").value;
}

std::vector<float> DAEAnimationLoader::getKeyTimes() {
    XNode timeData = m_animation.getChild("animation").getChild("source").getChild("float_array");
    return timeData.value.toFloatVector();
}

std::vector<KeyFrameData*> DAEAnimationLoader::initKeyFrames(std::vector<float> times) {
    std::vector<KeyFrameData*> frames;
    for (int i = 0; i < times.size(); i++) {
        frames.push_back(new KeyFrameData(times[i]));
    }
    return frames;
}

void
DAEAnimationLoader::loadJointTransforms(std::vector<KeyFrameData*> frames, XNode jointData, std::string rootNodeId) {
    std::string jointNameId = getJointName(jointData);
    std::string dataId = getDataId(jointData);
    XNode transformData = jointData.getNodeByAttribute("source", "id", dataId.c_str());
    std::vector<float> rawData = transformData.getChild("float_array").value.toFloatVector();
    processTransforms(jointNameId, rawData, frames, jointNameId == rootNodeId);

}

std::string DAEAnimationLoader::getJointName(XNode jointData) {
    XNode channelNode = jointData.getChild("channel");
    std::string data = channelNode.getAttribute("target").value;
    return split(data, '/')[0];
}

std::string DAEAnimationLoader::getDataId(XNode jointData) {
    XNode node = jointData.getChild("sampler").getNodeByAttribute("input", "semantic", "OUTPUT");
    return node.getAttribute("source").value.substr(1);
}

void DAEAnimationLoader::processTransforms(std::string jointName,
                                           std::vector<float> rawData, std::vector<KeyFrameData*> keyFrames, bool root) {

    for (int i = 0; i < keyFrames.size(); i++) {

        std::vector<float> matrixData;

        for (int j = 0; j < 16; j++) {
            matrixData.push_back(rawData[i * 16 + j]);
        }

        mat4 transform = mat4(&matrixData[0]);
        transform = transform.Transposed();

        if (root) {
            //because up axis in Blender is different to up axis in game
            transform *= CORRECTION;
        }

        keyFrames[i]->jointTransforms.push_back(new JointTransformData(RESMGR->GetStringId(jointName), jointName, transform));
    }

}
