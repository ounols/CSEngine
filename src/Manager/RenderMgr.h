#pragma once
#include "../MacroDef.h"
#include "RenderContainer.h"
#include "CoreBufferContainer.h"
#include "Base/RenderCoreBase.h"


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

    protected:
        void Exterminate() override;

    private:
        /**
         * Deferred rendering for each G-Buffer object.
         * @param camera Structure containing information values of the camera to be drawn.
         * @param gbuffer G-Buffer object to be drawn.
         */
        void RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const;
        /**
         * Deferred render all G-Buffers.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void RenderGbuffers(const CameraBase& camera) const;
        /**
         * Render the objects to be forward-rendered.
         * @param camera Structure containing information values of the camera to be drawn.
         * @param custom_handler Custom handler you want to draw. Applied as nullptr, except in special cases.
         */
        void RenderInstances(const CameraBase& camera, const GLProgramHandle* custom_handler = nullptr) const;
        /**
         * Rendering for a depth buffer to use for shadows. The render target depends on the shadow settings.
         * @param camera The light(camera) you want to render.
         * @param custom_handler Custom handler you want to draw. Applied shadow-related handlers mainly included in lighting.
         * @param render_objects List of objects to render
         */
        void RenderShadowInstance(const CameraBase& camera, const GLProgramHandle& custom_handler,
                                  const std::list<SIRender*>& render_objects = std::list<SIRender*>()) const;
        /**
         * Combine forward and deferred buffers and render to the desired camera.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void RenderBuffer(const CameraBase& camera) const;

    private:
        GLProgramHandle* m_mainProgramHandle = nullptr;
        GLProgramHandle* m_geometryHandle = nullptr;
    };
}