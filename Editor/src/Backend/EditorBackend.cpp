#include "EditorBackend.h"
#include "BackendUtils.h"
#include "../Manager/EditorAPIServer.h"
#include "../Manager/EditorActionLogger.h"
#include "../Manager/EEngineCore.h"
#include "../../../src/Util/AssetsDef.h"
#include "../../../src/Util/CaptureDef.h"

#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "../../../src/OGLDef.h"

namespace CSEditor {

    EditorBackend& EditorBackend::GetInstance() {
        static EditorBackend instance;
        return instance;
    }

    APIResponse EditorBackend::GetEditorInfo() {
        APIResponse response;

        auto* core = EEngineCore::getEditorInstance();
        std::ostringstream oss;
        oss << "{";
        oss << "\"version\":\"CSEditor 1.0\",";
        oss << "\"isPreview\":" << (core && core->IsPreview() ? "true" : "false") << ",";
        oss << "\"apiPort\":" << m_apiPort << ",";
        oss << "\"sessionStart\":\"" << EditorActionLogger::GetInstance().GetSessionStartTime() << "\"";
        oss << "}";

        response.body = oss.str();
        return response;
    }

    APIResponse EditorBackend::ExecuteCommand(const std::string& body) {
        APIResponse response;

        std::string command = BackendUtils::ParseJsonValue(body, "command");
        if (command.empty()) {
            response.statusCode = 400;
            response.body = "{\"error\":\"Missing command parameter\"}";
            return response;
        }

        ACTION_LOG_PARAMS(ActionCategory::EDITOR, ActionSeverity::INFO,
                         "API: Editor Command",
                         ActionParams().Set("command", command));

        if (command == "play") {
            auto* core = EEngineCore::getEditorInstance();
            if (core && !core->IsPreview()) {
                core->InvokePreviewStart(1280, 720);
                response.body = "{\"status\":\"ok\",\"command\":\"play\",\"resolution\":\"1280x720\"}";
            } else {
                response.body = "{\"status\":\"already_playing\"}";
            }
        } else if (command == "stop") {
            auto* core = EEngineCore::getEditorInstance();
            if (core && core->IsPreview()) {
                core->InvokePreviewStop();
                response.body = "{\"status\":\"ok\",\"command\":\"stop\"}";
            } else {
                response.body = "{\"status\":\"not_playing\"}";
            }
        } else {
            response.body = "{\"status\":\"queued\",\"command\":\"" + BackendUtils::EscapeJsonString(command) + "\"}";
        }

        return response;
    }

    APIResponse EditorBackend::CapturePreview(const std::string& queryParams) {
        APIResponse response;

        auto* core = EEngineCore::getEditorInstance();
        if (!core) {
            response.statusCode = 500;
            response.body = "{\"error\":\"Editor core not available\"}";
            return response;
        }

        std::string filename = BackendUtils::ParseQueryParam(queryParams, "filename");
        if (filename.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif
            std::ostringstream oss;
            oss << "preview_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".png";
            filename = oss.str();
        }

        if (filename.find(".png") == std::string::npos) {
            filename += ".png";
        }

        std::string resultPath;
        int resultWidth = 0, resultHeight = 0;
        bool success = false;

        {
            std::lock_guard<std::mutex> lock(m_previewCaptureMutex);
            m_pendingPreviewCaptures.push({
                filename,
                &resultPath,
                &resultWidth,
                &resultHeight,
                &success
            });
        }

        int waitCount = 0;
        while (waitCount < 100 && !success) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            waitCount++;
        }

        if (success) {
            ACTION_LOG_PARAMS(ActionCategory::SYSTEM, ActionSeverity::INFO,
                            "Preview captured",
                            ActionParams()
                                .Set("filename", filename)
                                .Set("width", std::to_string(resultWidth))
                                .Set("height", std::to_string(resultHeight)));

            std::ostringstream jsonResponse;
            jsonResponse << "{";
            jsonResponse << "\"success\":true,";
            jsonResponse << "\"filename\":\"" << filename << "\",";
            jsonResponse << "\"path\":\"" << resultPath << "\",";
            jsonResponse << "\"width\":" << resultWidth << ",";
            jsonResponse << "\"height\":" << resultHeight;
            jsonResponse << "}";

            response.statusCode = 200;
            response.body = jsonResponse.str();
        } else {
            response.statusCode = 500;
            response.body = "{\"error\":\"Failed to capture preview (timeout or error)\"}";
        }

        return response;
    }

    void EditorBackend::ProcessPendingOperations() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return;

        while (true) {
            PendingPreviewCapture capture;
            {
                std::lock_guard<std::mutex> lock(m_previewCaptureMutex);
                if (m_pendingPreviewCaptures.empty()) break;
                capture = m_pendingPreviewCaptures.front();
                m_pendingPreviewCaptures.pop();
            }

            unsigned int previewTextureId = core->GetPreviewTextureId();
            if (previewTextureId == 0) {
                *capture.success = false;
                continue;
            }

            glBindTexture(GL_TEXTURE_2D, previewTextureId);
            GLint width, height;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (width <= 0 || height <= 0) {
                *capture.success = false;
                continue;
            }

            GLuint captureFBO;
            glGenFramebuffers(1, &captureFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previewTextureId, 0);

            GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
                glDeleteFramebuffers(1, &captureFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                *capture.success = false;
                continue;
            }

            char* data = new char[width * height * 4];
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &captureFBO);

            std::string fullPath = CSE::NativeAssetsPath() + "../" + capture.filename;
            int saved = CSE::savePng(fullPath.c_str(), width, height, 4, data);
            delete[] data;

            *capture.resultPath = fullPath;
            *capture.resultWidth = width;
            *capture.resultHeight = height;
            *capture.success = (saved != 0);
        }
    }

    // ============================================
    // Direct Operations Implementation
    // ============================================

    bool EditorBackend::PlayDirect(int width, int height) {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return false;

        if (core->IsPreview()) {
            return false; // Already playing
        }

        core->InvokePreviewStart(width, height);

        ACTION_LOG_PARAMS(ActionCategory::EDITOR, ActionSeverity::INFO,
                         "Preview started directly",
                         ActionParams()
                             .Set("width", std::to_string(width))
                             .Set("height", std::to_string(height)));

        return true;
    }

    bool EditorBackend::StopDirect() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return false;

        if (!core->IsPreview()) {
            return false; // Not playing
        }

        core->InvokePreviewStop();

        ACTION_LOG_PARAMS(ActionCategory::EDITOR, ActionSeverity::INFO,
                         "Preview stopped directly",
                         ActionParams());

        return true;
    }

    void EditorBackend::ResizePreviewDirect(int width, int height) {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return;

        core->InvokePreviewResize(width, height);
    }

    bool EditorBackend::IsPlaying() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return false;

        return core->IsPreview();
    }

    unsigned int EditorBackend::GetPreviewTextureId() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return 0;

        return core->GetPreviewTextureId();
    }

    void EditorBackend::InvokeEditorRender() {
        auto* core = EEngineCore::getEditorInstance();
        if (!core) return;

        core->InvokeEditorRender();
    }

}
