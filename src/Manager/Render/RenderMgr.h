#pragma once
#include "../../MacroDef.h"
#include "RenderContainer.h"
#include "../CoreBufferContainer.h"
#include "../Base/RenderCoreBase.h"


namespace CSE {

    class CameraBase;
    class SGBuffer;

    /**
     * @class RenderMgr
     *
     * @brief Class for managing rendering operations.
     *
     * This class is responsible for managing all elements involved in the
     * rendering operation, including cameras, buffers, etc.
     */
    class RenderMgr : public RenderContainer, public CoreBufferContainer, public CoreBase, public RenderCoreBase {
    public:
        /**
         * @brief Constructor for RenderMgr.
         */
        explicit RenderMgr();
        ~RenderMgr() override;

    public:
        /**
         * @brief Initialization of resources required for rendering.
         */
        void Init() override;
        /**
         * @brief Modify the framebuffer to fit the modified screen size.
         *
         * @attention This is a function that can have a big impact on performance.
         */
        void SetViewport();
        /**
         * @brief Rendering all stuffs.
         */
        void Render() const override;

        /**
         * @brief Get the width of the screen.
         *
         * @return Pointer to the width of the screen.
         */
        unsigned int* GetWidth() const {
            return m_width;
        }

        /**
         * @brief Get the height of the screen.
         *
         * @return Pointer to the height of the screen.
         */
        unsigned int* GetHeight() const {
            return m_height;
        }

    protected:
        /**
         * @brief Destroy all resources used for rendering.
         */
        void Exterminate() override;

    private:
        /**
         * @brief Combine forward and deferred buffers and render to the desired camera.
         *
         * @param camera Structure containing information values of the camera to be drawn.
         */
        void ResetBuffer(const CameraBase& camera) const;

        /**
         * @brief Render shadows.
         */
        void RenderShadows() const;
        /**
         * @brief Render sub-cameras.
         */
        void RenderSubCameras() const;
        /**
         * @brief Render main camera.
         */
        void RenderMainCamera() const;
    };
}