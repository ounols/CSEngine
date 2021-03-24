#pragma once

#include <list>
#include <vector>
#include <cmath>

namespace CSE {

    class VertexSkinData {
    public:
        VertexSkinData() {}

        ~VertexSkinData() {}

        void addJointEffect(int jointId, float weight) {
            auto index = 0;
            auto jointID_it = m_jointIDs.begin();
            auto weight_it = m_weights.begin();
            for (const auto& weight_object : m_weights) {
                if (weight > weight_object) {
                    m_jointIDs.insert(jointID_it++, jointId);
                    m_weights.insert(weight_it++, weight);
                    return;
                }
            }
            m_jointIDs.push_back(jointId);
            m_weights.push_back(weight);
        }

        void limitJointNumber(int max) {
            if (m_jointIDs.size() > max) {
                std::vector<float> topWeights;
                topWeights.resize(max);
                float total = saveTopWeights(topWeights);
                refillWeightList(topWeights, total);
                removeExcessJointIds(max);
            } else if (m_jointIDs.size() < max) {
                fillEmptyWeights(max);
            }
        }


        const std::vector<int>& getJointIDs() const {
            return m_jointIDs;
        }

        const std::vector<float>& getWeights() const {
            return m_weights;
        }

    private:
        void fillEmptyWeights(int max) {
            m_jointIDs.reserve(max);
            m_weights.reserve(max);
            while (m_jointIDs.size() < max) {
                m_jointIDs.push_back(0);
                m_weights.push_back(0);
            }
        }

        float saveTopWeights(std::vector<float>& topWeightsArray) {
            float total = 0;
            auto size = topWeightsArray.size();
            for (int i = 0; i < size; ++i) {
                topWeightsArray[i] = m_weights[i];
                total += topWeightsArray[i];
            }
            return total;
        }

        void refillWeightList(std::vector<float> topWeights, float total) {
            m_weights.clear();
            m_weights.reserve(topWeights.size());
            for (const auto& topWeight : topWeights) {
                m_weights.push_back(std::fmin(topWeight / total, 1));
            }
        }

        void removeExcessJointIds(int max) {
            while (m_jointIDs.size() > max) {
                m_jointIDs.erase(m_jointIDs.begin() + m_jointIDs.size() - 1);
            }
        }

    private:
        std::vector<int> m_jointIDs;
        std::vector<float> m_weights;
    };
}