

#pragma once

#include "Base/WindowBase.h"
#include "Base/HierarchyData.h"

namespace CSEditor {
    class HierarchyWindow : public WindowBase, public HierarchyData {
    public:
        HierarchyWindow();

        ~HierarchyWindow() override;

        void SetUI() override;

    private:
        void RenderTrees();

        void RenderGameObject(CSE::SGameObject& parent);
    };
}