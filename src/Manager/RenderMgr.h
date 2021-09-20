#pragma once
#include "../MacroDef.h"
#include "RenderContainer.h"
#include "Base/RenderCoreBase.h"


namespace CSE {

    class SEnvironmentMgr;
    class CameraBase;
    class SGBuffer;

    class RenderMgr : public RenderContainer, public CoreBase, public RenderCoreBase {
    public:
        explicit RenderMgr();
        ~RenderMgr() override;

    public:
        void Init() override;
        void SetViewport();
        void Render() const override;

    protected:
        void Exterminate() override;

    private:
        void ResetBuffer(const CameraBase& camera) const;
        void RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const;
        void RenderGbuffers(const CameraBase& camera) const;
        void RenderInstances(const CameraBase& camera, const GLProgramHandle* custom_handler = nullptr) const;
        void RenderShadowInstance(const CameraBase& camera, const GLProgramHandle& custom_handler,
                                  const std::list<SIRender*>& render_objects = std::list<SIRender*>()) const;

    private:
        mat4 m_NoneCamera;

        SEnvironmentMgr* m_environmentMgr = nullptr;
        GLProgramHandle* m_geometryHandle = nullptr;
    };
}