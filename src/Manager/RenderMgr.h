#pragma once
#include "../MacroDef.h"
#include "RenderContainer.h"
#include "CameraMgr.h"
#include "Base/RenderCoreBase.h"
#include "../Util/Render/SEnvironmentMgr.h"


namespace CSE {

    class RenderMgr : public RenderContainer, public CoreBase, public RenderCoreBase {
    public:
        explicit RenderMgr();
        ~RenderMgr();

    public:
        void Init() override;

        void SetViewport(int width, int height);

        void Render() const override;

    private:
        void Exterminate();

    private:
        mat4 m_NoneCamera;
        int m_width = 1;
        int m_height = 1;

        SEnvironmentMgr* m_environmentMgr = nullptr;
    };
}