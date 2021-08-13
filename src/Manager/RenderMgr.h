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



    private:
        void RenderGbuffer(const CameraBase& camera) const;
        void RenderInstance(const CameraBase& camera, const GLProgramHandle* custom_handler = nullptr) const;
        void RenderShadowInstance(const CameraBase& camera, const GLProgramHandle& custom_handler,
                                  const std::list<SIRender*>& render_objects = std::list<SIRender*>()) const;

        void Exterminate();

    private:
        mat4 m_NoneCamera;
        unsigned int* m_width = nullptr;
        unsigned int* m_height = nullptr;

        SEnvironmentMgr* m_environmentMgr = nullptr;
        SGBuffer* m_gbufferObject = nullptr;
    };
}