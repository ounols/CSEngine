//
// Created by ounols on 19. 1. 15.
//

#pragma once

#include <vector>
#include <string>
#include "../../../MacroDef.h"

class SkinningData {
public:
    SkinningData() {}

    ~SkinningData() {
        for (auto skin : m_verticesSkinData) {
            SAFE_DELETE(skin);
        }

        m_verticesSkinData.clear();
    }

    const std::vector<std::string>& get_jointOrder() const {
        return m_jointOrder;
    }

    void set_jointOrder(const std::vector<std::string>& m_jointOrder) {
        SkinningData::m_jointOrder = m_jointOrder;
    }

    const std::vector<VertexSkinData*>& get_verticesSkinData() const {
        return m_verticesSkinData;
    }

    void set_verticesSkinData(const std::vector<VertexSkinData*>& m_verticesSkinData) {
        SkinningData::m_verticesSkinData = m_verticesSkinData;
    }

private:
    std::vector<std::string> m_jointOrder;

private:
    std::vector<VertexSkinData*> m_verticesSkinData;


};


