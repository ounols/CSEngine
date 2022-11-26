#pragma once
#include "../../MacroDef.h"
#include "RenderContainer.h"
#include "../CoreBufferContainer.h"
#include "../Base/RenderCoreBase.h"


namespace CSE {

    class CameraBase;
    class SGBuffer;

    class RenderMgr : public RenderContainer, public CoreBufferContainer, public CoreBase, public RenderCoreBase {
    public:
        explicit RenderMgr();
        ~RenderMgr() override;

    public:
        void Init() override;
        /**
         * @attention This is a function that can have a big impact on performance.
         *
         * Modify the framebuffer to fit the modified screen size.
         */
        void SetViewport();
        /**
         * Rendering all stuffs.
         */
        void Render() const override;

        unsigned int* GetWidth() const {
            return m_width;
        }

        unsigned int* GetHeight() const {
            return m_height;
        }

    protected:
        void Exterminate() override;

    private:
        /**
         * Combine forward and deferred buffers and render to the desired camera.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void ResetBuffer(const CameraBase& camera) const;
    };
}