#pragma once

#include "../../SObject.h"
#include "../Matrix.h"
#include "../Quaternion.h"
#include "../../Manager/ResMgr.h"
#include "../../MacroDef.h"
#include <utility>
#include <vector>
#include <string>

namespace CSE {

    class JointTransform {
    public:
        JointTransform() {
            m_position = vec3();
            m_rotation = Quaternion();
        }

        JointTransform(const vec3& position, Quaternion rotation) {
            m_position.Set(position.x, position.y, position.z);
            m_rotation = rotation;
        }

        ~JointTransform() = default;

        static JointTransform Interpolate(float t, JointTransform& a, JointTransform& b) {
            vec3 position = vec3::Lerp(t, a.m_position, b.m_position);
            Quaternion rotation = a.m_rotation.Slerp(t, b.m_rotation);

            return {position, rotation};
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

    class KeyFrame {
    public:
        KeyFrame(float timeStamp, std::vector<JointTransform*>&& pose) {
            m_timeStamp = timeStamp;
            m_pose = std::move(pose);
        }

        ~KeyFrame() {
            for (auto pair : m_pose) {
                SAFE_DELETE(pair);
            }

            m_pose.clear();
        }

        float GetTimeStamp() const {
            return m_timeStamp;
        }

        std::vector<JointTransform*> GetJointKeyFrames() const {
            return m_pose;
        }

    private:
        float m_timeStamp = 0;
        std::vector<JointTransform*> m_pose;
    };
}