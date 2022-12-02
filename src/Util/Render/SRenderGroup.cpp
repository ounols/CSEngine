#include "SRenderGroup.h"
#include "CameraBase.h"
#include "SFrameBuffer.h"
#include "../../Manager/Render/RenderMgr.h"
#include "STexture.h"
#include "../GLProgramHandle.h"

using namespace CSE;

void SRenderGroup::BindSourceBuffer(const SFrameBuffer& buffer, const GLProgramHandle& handle, int layout) {
    const auto& srcBufferTexture = buffer.GetMainColorTexture();
    const auto& uniforms = handle.Uniforms;
    const auto& size = buffer.GetSize();
    const float sizeRaw[2] = { static_cast<float>(size.x), static_cast<float>(size.y) };
    if(srcBufferTexture == nullptr) return;

    srcBufferTexture->Bind(uniforms.SourceBuffer, layout);
    glUniform2fv(uniforms.SourceBufferSize, 1, sizeRaw);
}
