#pragma once

#include "../../../src/Manager/EngineCoreInstance.h"

#define CORE CSEditor::EEngineCore::getInstance()

namespace CSEditor {

    class EPreviewCore;

    class HierarchyData;

    class ELogMgr;
}

namespace CSEditor {

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

        CSEditor::ELogMgr* GetLogMgrCore() const {
            return static_cast<CSEditor::ELogMgr*>(m_logMgr);
        }

        void BindPreviewFramebuffer() const;

        void InitPreviewFramebuffer();

        void StartPreviewCore();

        void StopPreviewCore();

        void UpdatePreviewCore();

        void RenderPreviewCore() const;

        void ResizePreviewCore();

        void UpdateTransforms();

        bool IsPreview() const {
            return m_previewCore != nullptr;
        }

        bool IsRender() {
            if(m_bIsRender) {
                m_bIsRender = false;
                return true;
            }
            return false;
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

        void InvokeEditorRender() {
            m_bIsRender = true;
        }

        InvokeState CheckInvokeState() {
            InvokeState result = m_invokeState;
            m_invokeState = InvokeState::NONE;
            return result;
        }

        unsigned int GetPreviewTextureId() const {
            return m_previewTextureId;
        }

        void SetHierarchyData(HierarchyData* data) {
            m_hierarchyData = data;
        }

        HierarchyData* GetHierarchyData() const {
            return m_hierarchyData;
        }

        void GenerateCores() override;

        void AddLog(const char* log, int category = 1);

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
        bool m_bIsRender = false;

        HierarchyData* m_hierarchyData = nullptr;
        ELogMgr* m_logMgr = nullptr;

    protected:
        static EEngineCore* sInstance;
    };
}
