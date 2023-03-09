#pragma once

#include "RenderInterfaces.h"
#include "SFrameBuffer.h"

namespace CSE {
    class SRenderGroup {
    public:
        SRenderGroup(const RenderMgr& renderMgr) : m_renderMgr(&renderMgr) {}
        virtual ~SRenderGroup() = default;

        virtual void RegisterObject(SIRender* object) = 0;

        virtual void RemoveObjects(SIRender* object) = 0;

        virtual void RenderAll(const CameraBase& camera) const = 0;

        virtual void Exterminate() = 0;

    protected:
        /**
         * 렌더링 직전의 소스 버퍼를 바인딩 하는 함수입니다.
         * @param buffer 바인딩 할 SFrameBuffer를 지정합니다.
         * @param handle SourceBuffer의 uniform id입니다.
         */
        static void BindSourceBuffer(const SFrameBuffer& buffer, const GLProgramHandle& handle, int layout);

    protected:
        const RenderMgr* m_renderMgr;
    };
}