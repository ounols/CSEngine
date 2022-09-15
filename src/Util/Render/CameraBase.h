#pragma once

namespace CSE {

    struct CameraMatrixStruct;
    class SFrameBuffer;

    class CameraBase {
    public:
        enum BackgroundType {
            NONE = 0, SOLID = 1, SKYBOX = 2
        };
    public:
        CameraBase() = default;
        virtual ~CameraBase() = default;

        virtual CameraMatrixStruct GetCameraMatrixStruct() const = 0;
        virtual SFrameBuffer* GetFrameBuffer() const = 0;
        virtual BackgroundType GetBackgroundType() {
            return SOLID;
        }
        virtual void RenderBackground() const = 0;
    };
}
