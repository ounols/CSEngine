#pragma once
#include "../MacroDef.h"
#include "RenderContainer.h"
#include "CameraMgr.h"

namespace CSE {

    class RenderMgr : public RenderContainer {
    public:
    DECLARE_SINGLETONE(RenderMgr);

        ~RenderMgr();

    public:
        void Init();

        void Render(float elapsedTime) const;

    private:
        void Exterminate();

    private:
        mat4 m_NoneCamera;
    };
}