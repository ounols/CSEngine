//
// Created by ounols on 19. 4. 18.
//

#include "../../OGLDef.h"
#include "STexture.h"
#include "stb_image.h"

#include "../../Manager/TextureContainer.h"
#include "../../Manager/ResMgr.h"

STexture::STexture() {
    ResMgr::getInstance()->Register<TextureContainer, STexture>(this);
}

STexture::~STexture() {

}

bool STexture::Load(const char* path, STexture::TYPE type) {

    if(m_id != 0) return false;

    std::vector<unsigned char> image;

    if(type == PNG) {
        unsigned error = lodepng::decode(image, m_width, m_height, path);
        if(error != 0) {
            return false;
        }
    }

    return Load(image, type);
}

bool STexture::Load(std::vector<unsigned char> data, STexture::TYPE type) {

    if(m_id != 0) return false;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return true;
}

bool STexture::LoadEmpty() {
    if(m_id != 0) return false;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    GLubyte data[] = { 255, 255, 255, 255 };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return true;
}


bool STexture::Reload(const char* path, TYPE type) {
    Release();
    Load(path, type);
}

bool STexture::Reload(std::vector<unsigned char> data, STexture::TYPE type) {
    Release();
    Load(data, type);
}

void STexture::Release() {
    glDeleteTextures(1, &m_id);
    m_id = 0;
    m_height = 0;
    m_width = 0;
}

void STexture::Exterminate() {
    Release();
//    ResMgr::getInstance()->Remove<TextureContainer, STexture>(this);
}

void STexture::Bind(const GLProgramHandle* handle) {
    if(m_id == 0) {
        LoadEmpty();
    }

    glBindTexture(GL_TEXTURE_2D, m_id);
    glUniform1i(handle->Uniforms.TextureSampler2D, GL_TEXTURE0);
}



