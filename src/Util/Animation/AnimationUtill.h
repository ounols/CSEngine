#pragma once

#include "../../SObject.h"
#include "../Matrix.h"
#include "../Quaternion.h"
#include "../../Manager/ResMgr.h"
#include "../../MacroDef.h"
#include <vector>
#include <map>
#include <string>

class KeyFrame;


class Animation : public SResource {
public:
    Animation(float totalTime, std::vector<KeyFrame*> keyframes) {
        SetUndestroyable(true);
        m_length = totalTime;
        m_keyframes = keyframes;
    }

    ~Animation() {
    }

    void Exterminate() override {
        for(auto keyframe : m_keyframes) {
            SAFE_DELETE(keyframe);
        }
        m_keyframes.clear();
    }

    float GetLength() const {
        return m_length;
    }

    std::vector<KeyFrame*> GetKeyFrames() const {
        return m_keyframes;
    }

private:
    float m_length = 0;
    std::vector<KeyFrame*> m_keyframes;

};


class JointTransform;

class KeyFrame {
public:
    KeyFrame(float timeStamp, std::map<std::string, JointTransform*> pose) {
        m_timeStamp = timeStamp;
        m_pose = pose;
    }

    ~KeyFrame() {
        for(auto pair : m_pose) {
            JointTransform* temp = pair.second;

            SAFE_DELETE(temp);
        }

        m_pose.clear();
    }

    float GetTimeStamp() const {
        return m_timeStamp;
    }

    std::map<std::string, JointTransform*> GetJointKeyFrames() const {
        return m_pose;
    }

private:
    float m_timeStamp = 0;
    std::map<std::string, JointTransform*> m_pose;
};

class JointTransform {
public:
    JointTransform() {
        m_position = vec3();
        m_rotation = Quaternion();
    }

    JointTransform(vec3 position, Quaternion rotation) {
        m_position.Set(position.x, position.y, position.z);
        m_rotation = rotation;
    }

    ~JointTransform() {

    }

    static JointTransform Interpolate(float t, JointTransform& a, JointTransform& b) {
        vec3 position = vec3::Lerp(t, a.m_position, b.m_position);
        Quaternion rotation = a.m_rotation.Slerp(t, b.m_rotation);

        return JointTransform(position, rotation);
    }

    mat4 GetLocalMatrix() {
        mat4 mat;
        mat = mat4::Translate(m_position.x, m_position.y, m_position.z);
        mat = m_rotation.ToMatrix4() * mat;


        return mat;
    }

private:
    vec3 m_position;
    Quaternion m_rotation;
};