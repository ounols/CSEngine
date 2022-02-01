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
        SFrameBuffer* GetForwardBuffer() const {
            return m_forwardBuffer;
        }
        SFrameBuffer* GetDeferredBuffer() const {
            return m_deferredBuffer;
        }

    protected:
        void InitBuffers(int width, int height);
        void ResizeBuffers(int width, int height);

    private:
        SFrameBuffer* m_mainBuffer = nullptr;
        SFrameBuffer* m_forwardBuffer = nullptr;
        SFrameBuffer* m_deferredBuffer = nullptr;
    };

}
