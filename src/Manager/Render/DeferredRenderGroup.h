#pragma once

#include <unordered_map>
#include "../../Util/Render/SRenderGroup.h"

namespace CSE {

    class SGBuffer;
    class LightMgr;

    /**
     * This class is responsible for deferred rendering of G-Buffer objects.
     */
    class DeferredRenderGroup : public SRenderGroup {
    private:
        typedef std::unordered_map<GLProgramHandle*, SGBuffer*> GBufferLayer;

    public:
        /**
         * Constructor for DeferredRenderGroup
         * @param renderMgr Reference to the RenderMgr object.
         */
        explicit DeferredRenderGroup(const RenderMgr& renderMgr);

        ~DeferredRenderGroup() override;

        /**
         * Register an object to the render group.
         * @param object A pointer to the object to be registered.
         */
        void RegisterObject(SIRender* object) override;

        /**
         * Remove an object from the render group.
         * @param object A pointer to the object to be removed.
         */
        void RemoveObjects(SIRender* object) override;

        /**
         * Deferred rendering for each G-Buffer object.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void RenderAll(const CameraBase& camera) const override;

        /**
         * Deferred rendering for each G-Buffer object.
         * @param camera Structure containing information values of the camera to be drawn.
         * @param gbuffer G-Buffer object to be drawn.
         */
        void RenderGbuffer(const CameraBase& camera, const SGBuffer& gbuffer) const;

        /**
         * Set the viewport size.
         */
        void SetViewport();

        /**
         * Free all allocated resources.
         */
        void Exterminate() override;

    private:
        GBufferLayer m_gbufferLayer;
        GLProgramHandle* m_geometryHandle = nullptr;

        unsigned int* m_width = nullptr;
        unsigned int* m_height = nullptr;

        LightMgr* m_lightMgr = nullptr;
    };
}