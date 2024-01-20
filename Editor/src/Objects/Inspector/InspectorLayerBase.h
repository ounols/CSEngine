#pragma once

namespace CSEditor {
    class InspectorLayerBase {
    public:
        InspectorLayerBase() = default;

        virtual ~InspectorLayerBase() = default;

        virtual void UpdateParams() = 0;

        virtual void RenderUI() = 0;

    protected:
        virtual void InitParams() = 0;
    };
}