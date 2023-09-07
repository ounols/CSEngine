#pragma once

#include <vector>

class XNode;

namespace CSE {
    class SComponent;
}

namespace CSEditor {

    class InspectorParam;

    class InspectorLayer {
    public:
        explicit InspectorLayer(CSE::SComponent& component);

        ~InspectorLayer();
        void UpdateParams();
        void RenderUI();

    private:
        void InitParams();

    private:
        std::vector<InspectorParam*> m_params;
        CSE::SComponent* m_component = nullptr;
    };
}