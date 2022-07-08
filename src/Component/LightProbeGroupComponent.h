#pragma once

#include "SComponent.h"
#include "../Util/Render/RenderInterfaces.h"
#include <vector>

namespace CSE {

//    class LightProbeData;
    class LightMgr;
    struct CameraMatrixStruct;

//    struct LightProbeResultData {
//        unsigned int m_textures[3];
//        float m_weights[3];
//    };

    class LightProbeGroupComponent : public SComponent, public SIRenderGroup {
    private:
        struct LightProbeCameraNode {
            vec3 position;
            Vector2<unsigned short> uv;
        };
    public:
        LightProbeGroupComponent();
        ~LightProbeGroupComponent() override;

        void InitProbeTexture(Vector3<unsigned short> m_nodeCount);
        void RenderAll(unsigned int framebufferId, const std::list<SIRender*>& renderList) const override;
        void RenderInstances(const std::list<SIRender*>& renderList) const;

        void Init() override;

        void Tick(float elapsedTime) override;

        void Exterminate() override;

    private:
        void GenerateCameraNodeList();

    private:
        const unsigned short PER_WIDTH = 50;
        std::vector<LightProbeCameraNode*> m_nodes;
        Vector3<unsigned short> m_size;
        unsigned int m_texId;
        mutable unsigned short m_renderOffset = 0;
        unsigned short m_renderSnap;

        vec3* m_pivot = nullptr;
        vec3* m_scale = nullptr;
        vec3 m_nodeOffset;

        CameraMatrixStruct* m_cameraMatrixStruct = nullptr;
        mat4 m_frontCamera;
        mat4 m_backCamera;

        LightMgr* m_lightMgr = nullptr;
    };
}
