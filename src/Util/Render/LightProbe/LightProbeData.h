#pragma once

namespace CSE {

    class SProbeTexture;

    struct LightProbeData {
        SProbeTexture* m_texture;
        vec3 m_position;
        bool m_enable;
    };
}