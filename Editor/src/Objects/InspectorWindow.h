#pragma once

#include <string>
#include <vector>
#include "Base/WindowBase.h"

class XNode;

namespace CSE {
    class SGameObject;
    class SComponent;
}

namespace CSEditor {

    class EEngineCore;
    class InspectorLayerBase;

    class InspectorWindow : public WindowBase {
    public:
        InspectorWindow();

        ~InspectorWindow() override;

        void SetUI() override;

        void ReleaseLayers();

    private:
        void InitLayers(const CSE::SGameObject& object);

        void RenderGameObjectHeader(CSE::SGameObject* object);

        void RenderAddComponentMenu(CSE::SGameObject* object);

        void RemoveComponent(CSE::SGameObject* object, CSE::SComponent* component);

    private:
        EEngineCore* m_core = nullptr;
        CSE::SGameObject* m_selectedPrev = nullptr;
        std::vector<InspectorLayerBase*> m_layers;
        char m_nameBuffer[128] = {0};
        bool m_showAddComponentPopup = false;
    };
}