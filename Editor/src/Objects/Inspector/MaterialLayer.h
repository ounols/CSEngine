#pragma once

#include <vector>
#include "InspectorLayerBase.h"
#include "../../../src/Util/STypeDef.h"

namespace CSE {
    class SMaterial;
    class RenderComponent;
}

namespace CSEditor {

    class InspectorParam;

    class MaterialLayer : public InspectorLayerBase {
    public:
        explicit MaterialLayer(CSE::RenderComponent& component);

        ~MaterialLayer() override;

        void UpdateParams() override;

        void RenderUI() override;

    protected:
        void InitParams() override;

        static const char* ConvertSTypeToParamType(CSE::SType type);
        static const char* ConvertSTypeToClassType(CSE::SType type);

    private:
        std::vector<InspectorParam*> m_params;
        CSE::SMaterial* m_material = nullptr;
        CSE::SMaterial* m_material_ref = nullptr;
        CSE::RenderComponent* m_render = nullptr;
    };
}