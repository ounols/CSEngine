#pragma once

#include <unordered_map>
#include "../../Util/Render/SRenderGroup.h"

namespace CSE {

    class LightMgr;

    /**
     * This class is used for forward-rendering objects.
     * It contains a collection of objects which should be forward-rendered.
     */
    class ForwardRenderGroup : public SRenderGroup {
    private:
        typedef std::vector<SIRender*> RenderInterfaces;
        typedef std::unordered_map<GLProgramHandle*, RenderInterfaces> ProgramRenderLayer;
        typedef std::unordered_map<short, ProgramRenderLayer> OrderRenderLayer;

    public:
        /**
         * Constructor for the ForwardRenderGroup class.
         * @param renderMgr The render manager object.
         */
        explicit ForwardRenderGroup(const RenderMgr& renderMgr);

        /**
         * Destructor for the ForwardRenderGroup class.
         */
        ~ForwardRenderGroup() override;

        /**
         * Register an object to be forward-rendered.
         * @param object The object to be registered.
         */
        void RegisterObject(SIRender* object) override;

        /**
         * Remove an object from the forward-rendering list.
         * @param object The object to be removed.
         */
        void RemoveObjects(SIRender* object) override;

        /**
         * Render the objects to be forward-rendered.
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void RenderAll(const CameraBase& camera) const override;

        /**
         * Clear all objects in the forward-rendering list.
         */
        void Exterminate() override;

    private:
        OrderRenderLayer m_rendersLayer;
        LightMgr* m_lightMgr = nullptr;
    };

}