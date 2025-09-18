#pragma once

#include <string>
#include <vector>
#include "Base/WindowBase.h"

class XNode;

namespace CSE {
    class SGameObject;
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

    private:
        EEngineCore* m_core = nullptr;
        CSE::SGameObject* m_selectedPrev = nullptr;
        std::vector<InspectorLayerBase*> m_layers;
    };
}