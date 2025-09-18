//
// Created by ounols on 19. 1. 20.
//

#pragma once

#include "../../Manager/ResMgr.h"
#include "Joint.h"

namespace CSE {

    class Skeleton : public SResource {
    public:
        RESOURCE_DEFINE_CONSTRUCTOR(Skeleton);

        Skeleton(int jointCount, Joint* headJoint) : m_jointCount(jointCount), m_headJoint(headJoint),
                                                     SResource("Skeleton") {
            SetUndestroyable(true);
        }

        ~Skeleton() override {
            Exterminate();
        }

        void SetJoint(int jointCount, Joint* headJoint) {
            m_jointCount = jointCount;
            m_headJoint = headJoint;
        }


        int getJointCount() const {
            if (!isLoaded()) return -1;
            return m_jointCount;
        }

        void setJointCount(int jointCount) {
            Skeleton::m_jointCount = jointCount;
        }

        Joint* getHeadJoint() const {
            if (!isLoaded()) return nullptr;
            return m_headJoint;
        }

        void setHeadJoint(Joint* headJoint) {
            Skeleton::m_headJoint = headJoint;
        }

        void Exterminate() override {
            SAFE_DELETE(m_headJoint);
        }

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override {
            return;
        }

    private:
        bool isLoaded() const {
            return m_jointCount > 0 && m_headJoint != nullptr;
        }

    private:
        int m_jointCount;
        Joint* m_headJoint = nullptr;
    };

}