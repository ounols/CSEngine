#pragma once
#include "../MacroDef.h"
#include "RenderContainer.h"
#include "CameraMgr.h"
#include "Base/RenderCoreBase.h"

namespace CSE {

    class RenderMgr : public RenderContainer, public CoreBase, public RenderCoreBase {
    public:
        explicit RenderMgr();
        ~RenderMgr();

    public:
        void Init() override;

        void Render() const override;

    private:
        void Exterminate();

    private:
        mat4 m_NoneCamera;
    };
}