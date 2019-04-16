//
// Created by ounols on 19. 1. 20.
//

#pragma once

#include <string>
#include <vector>

#include "../../MacroDef.h"
#include "../Matrix.h"


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
        if (child == nullptr) return;
        m_children.push_back(child);
    }

    const std::string& GetName() const {
        return m_nameID;
    }

    int GetIndex() const {
        return m_index;
    }

    const mat4 GetBindLocalTransform() const {
        return m_bindLocalTransform;
    }

    const std::vector<Joint*>& GetChildren() const {
        return m_children;
    }

private:
    int m_index;
    std::string m_nameID;
    mat4 m_bindLocalTransform;

    std::vector<Joint*> m_children;

};


