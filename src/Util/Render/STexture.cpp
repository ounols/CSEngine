//
// Created by ounols on 19. 4. 18.
//
#define STB_IMAGE_IMPLEMENTATION

#include "../../OGLDef.h"
#include "STexture.h"

#include "../Loader/STB/stb_image.h"
#include "../../Manager/ResMgr.h"
#include "../AssetsDef.h"

using namespace CSE;

STexture::STexture() {
    SetUndestroyable(true);
}

STexture::~STexture() {

}

bool STexture::LoadFile(const char* path) {

    if (m_texId != 0) return false;

//    m_name = path;
    unsigned char* data = stbi_load(path, &m_width, &m_height, &m_channels, 0);

    return Load(data);
}

bool STexture::Load(unsigned char* data) {

    if (m_texId != 0) {
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);

    m_channels = GL_RGB;
    if (m_channels == 4) m_channels = GL_RGBA;

    m_internalFormat = m_channels;
    glTexImage2D(GL_TEXTURE_2D, 0, m_channels, m_width, m_height, 0, m_internalFormat, m_glType, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return true;
}

bool STexture::LoadEmpty() {
    if (m_texId != 0) return false;

    m_width = 1;
    m_height = 1;
    m_channels = GL_RGBA;
    m_internalFormat = GL_RGBA;
    m_glType = GL_UNSIGNED_BYTE;

    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);

    GLubyte data[] = { 255, 255, 255, 255 };

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_channels, m_glType, data);

    return true;
}


bool STexture::ReloadFile(const char* path) {
    Release();
    return LoadFile(path);
}

bool STexture::Reload(unsigned char* data) {
    Release();
    return Load(data);
}

void STexture::Release() {
    glDeleteTextures(1, &m_texId);
    m_texId = 0;
    m_height = 0;
    m_width = 0;
}

void STexture::Exterminate() {
    Release();
//    ResMgr::getInstance()->Remove<TextureContainer, STexture>(this);
}

void STexture::Bind(GLint location, int layout) {
    if (m_texId == 0) {
        LoadEmpty();
    }
    glUniform1i(location, layout);

    glActiveTexture(GL_TEXTURE0 + layout);
    glBindTexture(GL_TEXTURE_2D, m_texId);
}

bool STexture::InitTexture(int width, int height, int channel, int internalFormat, int glType) {
    if (m_texId != 0) {
        return false;
    }

    m_width = width;
    m_height = height;
    m_channels = channel;
    m_internalFormat = internalFormat;
    m_glType = glType;

    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_channels, m_glType, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void STexture::SetParameteri(int targetName, int value) const {
    glBindTexture(GL_TEXTURE_2D, m_texId);
    glTexParameteri(GL_TEXTURE_2D, targetName, value);
}

void STexture::SetParameterfv(int targetName, float* value) const {
    glBindTexture(GL_TEXTURE_2D, m_texId);
    glTexParameterfv(GL_TEXTURE_2D, targetName, value);
}

void STexture::Init(const AssetMgr::AssetReference* asset) {
	const std::string img_str = CSE::AssetMgr::LoadAssetFile(asset->path);

    auto data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(img_str.c_str()), img_str.length(),
                                      &m_width, &m_height, &m_channels, 0);
    Load(data);

}



