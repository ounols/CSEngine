#pragma once

namespace CSE {

    struct CameraMatrixStruct;
    class SFrameBuffer;

    class CameraBase {
    public:
        CameraBase() = default;
        virtual ~CameraBase() = default;

        virtual CameraMatrixStruct GetCameraMatrixStruct() const = 0;
        virtual SFrameBuffer* GetFrameBuffer() const = 0;
    };
}
