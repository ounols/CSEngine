

#pragma once

#include "Base/WindowBase.h"
#include "Base/HierarchyData.h"

namespace CSEditor {

    class EEngineCore;

    class HierarchyWindow : public WindowBase, public HierarchyData {
    public:
        HierarchyWindow();

        ~HierarchyWindow() override;

        void SetUI() override;

        // GameObject management
        CSE::SGameObject* CreateEmptyGameObject(CSE::SGameObject* parent = nullptr);
        CSE::SGameObject* CreatePrimitiveGameObject(const char* name, CSE::SGameObject* parent = nullptr);
        void DeleteSelectedGameObject();
        void DuplicateSelectedGameObject();

    private:
        void RenderTrees();

        void RenderGameObject(CSE::SGameObject& parent);

        void UpdateGameObject(CSE::SGameObject& parent);

        void RenderContextMenu();

        void RenderCreateMenu(CSE::SGameObject* parent);

        void HandleKeyboardShortcuts();

    private:
        EEngineCore* m_core = nullptr;
        bool m_showContextMenu = false;
        CSE::SGameObject* m_contextMenuTarget = nullptr;
    };
}