#include "SProbeTexture.h"
#include "../../../OGLDef.h"

using namespace CSE;

SProbeTexture::SProbeTexture() {

}

SProbeTexture::~SProbeTexture() {

}

bool SProbeTexture::LoadFromMemory(const unsigned char* rawData, int length) {
    return false;
}

void SProbeTexture::InitTexture(int width) {
    m_width = width;
    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);
    for (unsigned short i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB565, m_width, m_width, 0, 3,
                     GL_UNSIGNED_BYTE, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
