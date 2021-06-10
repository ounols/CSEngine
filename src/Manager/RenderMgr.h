#pragma once
#include "../MacroDef.h"
#include "RenderContainer.h"
#include "Base/RenderCoreBase.h"


namespace CSE {

    class SEnvironmentMgr;
    class CameraBase;

    class RenderMgr : public RenderContainer, public CoreBase, public RenderCoreBase {
    public:
        explicit RenderMgr();
        ~RenderMgr();

    public:
        void Init() override;

        void SetViewport(int width, int height);

        void Render() const override;



    private:
        void RenderInstance(const CameraBase& camera, const GLProgramHandle* custom_handler = nullptr) const;
        void RenderShadowInstance(const CameraBase& camera, const GLProgramHandle* handle) const;

        void Exterminate();

    private:
        mat4 m_NoneCamera;
        int m_width = 1;
        int m_height = 1;

        SEnvironmentMgr* m_environmentMgr = nullptr;
    };
}