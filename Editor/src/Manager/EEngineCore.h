#pragma once

#include "../../../src/Manager/EngineCoreInstance.h"

#define CORE CSEditor::EEngineCore::getInstance()

namespace CSEditor {

    class EPreviewCore;

    class EEngineCore : public CSE::EngineCoreInstance {
    public:
        enum InvokeState {
            NONE = 0, START, RESIZE, STOP
        };
    private:
        EEngineCore(const EEngineCore&);

        EEngineCore& operator=(const EEngineCore&);

        ~EEngineCore() override;

    protected:
        explicit EEngineCore();

    public:
        static CSE::EngineCoreInstance* getInstance();
        static EEngineCore* getEditorInstance();

        static void delInstance();

    public:
        void BindPreviewFramebuffer() const;

        void InitPreviewFramebuffer();

        void StartPreviewCore();

        void StopPreviewCore();

        void UpdatePreviewCore();

        void RenderPreviewCore() const;

        void ResizePreviewCore();

        bool IsPreview() const {
            return m_previewCore != nullptr;
        }

        EPreviewCore* GetPreviewCore() const {
            return m_previewCore;
        }

        void InvokePreviewStart(unsigned int width, unsigned int height) {
            m_invokeState = InvokeState::START;
            m_previewWidth = width;
            m_previewHeight = height;
        }

        void InvokePreviewResize(unsigned int width, unsigned int height) {
            m_invokeState = InvokeState::RESIZE;
            m_previewWidth = width;
            m_previewHeight = height;
        }

        void InvokePreviewStop() {
            m_invokeState = InvokeState::STOP;
        }

        InvokeState CheckInvokeState() {
            InvokeState result = m_invokeState;
            m_invokeState = InvokeState::NONE;
            return result;
        }

        unsigned int GetPreviewTextureId() const {
            return m_previewTextureId;
        }

    private:
        static long long int GetCurrentMillis();

        void ResizePreviewFramebuffer(unsigned int width, unsigned int height);

    private:
        EPreviewCore* m_previewCore = nullptr;
        InvokeState m_invokeState = InvokeState::NONE;

        long long int m_startTime = 0;
        float m_previewElapsedTime = 0.f;

        unsigned int m_previewFbo = 0;
        unsigned int m_previewTextureId = 0;
        unsigned int m_previewWidth = 0;
        unsigned int m_previewHeight = 0;

    protected:
        static EEngineCore* sInstance;
    };

}