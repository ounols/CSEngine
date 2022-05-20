#pragma once

#include "SComponent.h"
#include <list>

namespace CSE {

    class LightProbeData;

    struct LightProbeResultData {
        unsigned int m_textures[3];
        float m_weights[3];
    };

    class LightProbeGroupComponent : public SComponent {
    public:
        LightProbeGroupComponent();
        ~LightProbeGroupComponent() override;

        void Render(unsigned int framebufferId) const;
        LightProbeResultData GetLightProbe(const vec3& position) const;

    private:
        std::list<LightProbeData*> m_nodes;

    };
}
