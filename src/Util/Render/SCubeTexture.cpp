//
// Created by ounols on 19. 5. 4.
//

#include "SCubeTexture.h"
#include "../../OGLDef.h"


#include "../../Manager/TextureContainer.h"
#include "../../Manager/ResMgr.h"

SCubeTexture::SCubeTexture() {

}

SCubeTexture::~SCubeTexture() {

}

bool SCubeTexture::CreateCubeTexture(int size) {
    if (m_id != 0) {
        return false;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    GLuint channel = GL_RGB;
    if (m_channels == 4) channel = GL_RGBA;

    for (GLuint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, channel, size, size, 0, channel, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void SCubeTexture::Bind(GLint location, int layout) {
    if (m_id == 0) {
        return;
    }

    glUniform1i(location, layout);

    glActiveTexture(GL_TEXTURE0 + layout);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

}

void SCubeTexture::GenerateMipmap() {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
