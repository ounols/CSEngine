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

        void InitProbeTexture(Vector3<unsigned short> m_size);
        void Render(unsigned int framebufferId) const;

    private:
        const unsigned short PER_WIDTH = 3;
        //std::list<LightProbeData*> m_nodes;
        Vector3<unsigned short> m_size;
        unsigned int m_texId;

        vec3* m_pivot = nullptr;
        vec3* m_scale = nullptr;
        vec3 m_nodeOffset;
    };
}
