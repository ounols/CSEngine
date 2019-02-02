//
// Created by ounols on 19. 1. 20.
//

#pragma once

#include <string>
#include <vector>

#include "Util/Matrix.h"


class Joint {
public:
    Joint(int m_index, std::string m_nameID, mat4 m_bindLocalTransform)
            : m_index(m_index), m_nameID(m_nameID), m_bindLocalTransform(m_bindLocalTransform) {

    }
    ~Joint() {
        for (auto child : m_children) {
            SAFE_DELETE(child);
        }

        m_children.clear();
    }

    void addChild(Joint* child) {
        if(child == nullptr) return;
        m_children.push_back(child);
    }

private:
    int m_index;
    std::string m_nameID;
    mat4 m_bindLocalTransform;

    std::vector<Joint*> m_children;

};


