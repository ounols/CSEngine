#pragma once

#include <vector>
#include "InspectorLayerBase.h"

class XNode;

namespace CSE {
    class SComponent;
}

namespace CSEditor {

    class InspectorParam;

    class InspectorLayer : public InspectorLayerBase {
    public:
        explicit InspectorLayer(CSE::SComponent& component);

        ~InspectorLayer() override;
        void UpdateParams() override;
        void RenderUI() override;

    private:
        void InitParams() override;

    private:
        std::vector<InspectorParam*> m_params;
        CSE::SComponent* m_component = nullptr;
    };
}