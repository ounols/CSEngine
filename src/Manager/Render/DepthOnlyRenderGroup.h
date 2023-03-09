#pragma once

#include "../../Util/Render/SRenderGroup.h"
#include <list>

namespace CSE {

    class SShaderGroup;

    /**
     * This class is responsible for rendering depth only objects for use in creating shadow maps.
     */
    class DepthOnlyRenderGroup : public SRenderGroup {
    private:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::unordered_map<SShaderGroup*, RenderInterfaces> ProgramRenderLayer;

    public:
        /**
         * Constructor for the DepthOnlyRenderGroup.
         *
         * @param renderMgr The render manager to use.
         */
        explicit DepthOnlyRenderGroup(const RenderMgr& renderMgr);

        /**
         * Register an object to the render group.
         *
         * @param object The object to be registered.
         */
        void RegisterObject(SIRender* object) override;

        /**
         * Remove an object from the render group.
         *
         * @param object The object to be removed.
         */
        void RemoveObjects(SIRender* object) override;

        /**
         * Rendering for a depth buffer to use for shadows. The render target depends on the shadow settings.
         * @param camera The light(camera) you want to render.
         */
        void RenderAll(const CameraBase& camera) const override;

        /**
         * Removes all objects from the render group.
         */
        void Exterminate() override;

    private:
        ProgramRenderLayer m_depthObjects;
    };
}