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

    protected:
        void InitBuffers(int width, int height);
        void ResizeBuffers(int width, int height);

    private:
        SFrameBuffer* m_mainBuffer = nullptr;
    };

}
