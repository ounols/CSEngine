#pragma once

namespace CSE {

    class SFrameBuffer;

    class CoreBufferContainer {
    public:
        CoreBufferContainer() = default;
        virtual ~CoreBufferContainer() = default;

        SFrameBuffer* GetMainBuffer() const {
            return m_mainBuffer;
        }

        void SetDeviceBuffer(unsigned int id) {
            m_deviceBuffer = id;
        }

    protected:
        void InitBuffers(int width, int height);
        void ResizeBuffers(int width, int height);

    protected:
        unsigned int m_deviceBuffer = 0;

    private:
        SFrameBuffer* m_mainBuffer = nullptr;
    };

}
