#include "SRenderGroup.h"
#include "CameraBase.h"
#include "SFrameBuffer.h"
#include "../../Manager/Render/RenderMgr.h"
#include "STexture.h"

using namespace CSE;

void SRenderGroup::BindSourceBuffer(const SFrameBuffer& buffer, int uniformId, int layout) {
    if(uniformId < 0) return;
    const auto& srcBufferTexture = buffer.GetMainColorTexture();
    if(srcBufferTexture == nullptr) return;

    srcBufferTexture->Bind(uniformId, layout);
}
