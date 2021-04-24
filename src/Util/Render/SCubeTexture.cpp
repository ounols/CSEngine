//
// Created by ounols on 19. 5. 4.
//

#include "SCubeTexture.h"
#include "../../OGLDef.h"

#include "../../Manager/ResMgr.h"

using namespace CSE;

SCubeTexture::SCubeTexture() {

}

SCubeTexture::~SCubeTexture() {

}

bool SCubeTexture::InitTexture(int width, int height, int channel, int internalFormat, int glType) {
    if (m_texId != 0) {
        return false;
    }

    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);

    for (GLuint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, channel, glType, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

bool SCubeTexture::InitTextureMipmap(int width, int height, int channel, int internalFormat, int glType) {
    if (m_texId != 0) {
        return false;
    }

    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);

    for (GLuint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, channel, glType, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void SCubeTexture::Bind(GLint location, int layout) {
    if (m_texId == 0) {
        return;
    }

    glUniform1i(location, layout);

    glActiveTexture(GL_TEXTURE0 + layout);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);

}

void SCubeTexture::GenerateMipmap() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void SCubeTexture::Init(const AssetMgr::AssetReference* asset) {
    STexture::Init(asset);
}
