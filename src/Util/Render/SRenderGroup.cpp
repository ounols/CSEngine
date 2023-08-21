#include "SRenderGroup.h"
#include "CameraBase.h"
#include "SFrameBuffer.h"
#include "../../Manager/Render/RenderMgr.h"
#include "STexture.h"
#include "../GLProgramHandle.h"

using namespace CSE;

void SRenderGroup::BindSourceBuffer(const SFrameBuffer& buffer, const GLProgramHandle& handle, int layout) {
    const auto& uniforms = handle.Uniforms;
    const auto& size = buffer.GetSize();
    const float sizeRaw[2] = { static_cast<float>(size.x), static_cast<float>(size.y) };

    if(uniforms.SourceBuffer >= 0) {
        const auto& texture = buffer.BlitCopiedFrameBuffer();

        buffer.AttachFrameBuffer();
        texture->Bind(uniforms.SourceBuffer, layout);
    }
    if(uniforms.SourceBufferSize >= 0)
        glUniform2fv(uniforms.SourceBufferSize, 1, sizeRaw);
}
