#pragma once

namespace CSE {
    class SGameObject;
}

namespace CSEditor {
    class HierarchyData {
    public:
        HierarchyData() = default;
        virtual ~HierarchyData() = default;

        CSE::SGameObject* GetSelectedObject() const {
            return m_selected;
        }

        void ClearSelectedObject() {
            m_selected = nullptr;
        }

    protected:
        CSE::SGameObject* m_selected = nullptr;
    };
}