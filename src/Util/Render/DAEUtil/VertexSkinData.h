#pragma once

#include <list>
#include <vector>
#include <cmath>

class VertexSkinData {
public:
    VertexSkinData() {}

    ~VertexSkinData() {}

    void addJointEffect(int jointId, float weight) {
        for (int i = 0; i < m_weights.size(); i++) {
            if (weight > m_weights[i]) {
                auto jointID_it = m_jointIDs.begin();
                auto weight_it = m_weights.begin();
                m_jointIDs.insert(jointID_it + i, jointId);
                m_weights.insert(weight_it + i, weight);
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
        while (m_jointIDs.size() < max) {
            m_jointIDs.push_back(0);
            m_weights.push_back(0);
        }
    }

    float saveTopWeights(std::vector<float> topWeightsArray) {
        float total = 0;
        for (int i = 0; i < topWeightsArray.size(); i++) {
            topWeightsArray[i] = m_weights.at(i);
            total += topWeightsArray[i];
        }
        return total;
    }

    void refillWeightList(std::vector<float> topWeights, float total) {
        m_weights.clear();
        for (int i = 0; i < topWeights.size(); i++) {
            m_weights.push_back(std::fminf(topWeights[i] / total, 1));
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