//
// Created by ounols on 19. 4. 18.
//
#define STB_IMAGE_IMPLEMENTATION

#include "../../OGLDef.h"
#include "STexture.h"

#include "../Loader/STB/stb_image.h"
#include "../Loader/XML/XML.h"
#include "../../MacroDef.h"
#include "../AssetsDef.h"

using namespace CSE;

unsigned int STexture::m_emptyTextureId = 0;

STexture::STexture() {
    SetUndestroyable(true);
    if(m_emptyTextureId == 0) {
        LoadEmpty();
    }
}

STexture::STexture(STexture::Type type) {
    SetUndestroyable(true);
    SetType(type);
}

STexture::~STexture() = default;

bool STexture::LoadFile(const char* path) {

    if (m_texId != 0) return false;

//    m_name = path;
    unsigned char* data = stbi_load(path, &m_width, &m_height, &m_channels, 0);

    return Load(data);
}

bool STexture::LoadFromMemory(const unsigned char* rawData, int length) {
    unsigned char* data;
    if(m_type == TEX_3D) {
        int w, h;
        data = stbi_load_from_memory(rawData, length, &w, &h, &m_channels, 0);
    }
    else
        data = stbi_load_from_memory(rawData, length, &m_width, &m_height, &m_channels, 0);
    return Load(data);
}

bool STexture::Load(unsigned char* data) {

    if (m_texId != 0) {
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &m_texId);
    glBindTexture(m_targetGL, m_texId);

    m_internalFormat = GL_RGB;
    switch (m_channels) {
        case 1:
            m_internalFormat = GL_R8;
            break;
        case 2:
            m_internalFormat = GL_RG;
            break;
        case 4:
            m_internalFormat = GL_RGBA;
            break;
    }

    switch (m_type) {
        case TEX_2D:
            glTexImage2D(m_targetGL, 0, m_internalFormat, m_width, m_height, 0, m_internalFormat, m_glType, data);
            break;
        case TEX_CUBE:
            glTexImage2D(m_targetGL, 0, m_internalFormat, m_width, m_height, 0, m_internalFormat, m_glType, data);
            break;
        case TEX_3D:
            glTexImage3D(m_targetGL, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_internalFormat, m_glType, data);
            break;
    }

    glTexParameteri(m_targetGL, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_targetGL, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_targetGL, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(m_targetGL, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_targetGL, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return true;
}

void STexture::LoadEmpty() {
    if (m_emptyTextureId != 0) return;

    glGenTextures(1, &m_emptyTextureId);
    glBindTexture(GL_TEXTURE_2D, m_emptyTextureId);

    GLubyte data[] = { 255, 0, 255 };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
}

void STexture::Bind(GLint location, int layout) {
    if (m_texId == 0) {
        BindEmpty(location, layout, m_type);
        return;
    }
    glUniform1i(location, layout);

    glActiveTexture(GL_TEXTURE0 + layout);
    glBindTexture(m_targetGL, m_texId);
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
    glBindTexture(m_targetGL, m_texId);

    switch (m_type) {
        case TEX_CUBE:
            for (GLuint i = 0; i < 6; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, m_width, m_height, 0, m_channels,
                             m_glType, nullptr);
            }
            break;

        case TEX_2D:
            glTexImage2D(m_targetGL, 0, m_internalFormat, m_width, m_height, 0, m_channels, m_glType, nullptr);
            break;

        case TEX_3D:
            glTexImage3D(m_targetGL, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_channels, m_glType, nullptr);
            break;
    }

    glTexParameteri(m_targetGL, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_targetGL, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_targetGL, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_targetGL, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_targetGL, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

bool STexture::InitTextureMipmap(int width, int height, int channel, int internalFormat, int glType) {
    auto result = InitTexture(width, height, channel, internalFormat, glType);
    if(!result) return false;
    glTexParameteri(m_targetGL, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    return result;
}

void STexture::SetParameteri(int targetName, int value) const {
    glBindTexture(m_targetGL, m_texId);
    glTexParameteri(m_targetGL, targetName, value);
}

void STexture::SetParameterfv(int targetName, float* value) const {
    glBindTexture(m_targetGL, m_texId);
    glTexParameterfv(m_targetGL, targetName, value);
}

void STexture::Init(const AssetMgr::AssetReference* asset) {
	const std::string img_str = CSE::AssetMgr::LoadAssetFile(asset->name_path);

    std::string hashRaw = AssetMgr::LoadAssetFile(asset->name_path + ".meta");
    if(!hashRaw.empty()) {
        const XNode* root = XFILE().loadBuffer(hashRaw);
        const auto& hashData = root->getNode("hash-data");
        std::string hash = hashData.getAttribute("hash").value;
        SetHash(hash);
        const auto& hashChildren = hashData.children;

        if(hashData.children.size() <= 0) {
            hashRaw = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                      "<CSEMETA version=\"1.0.0\">\n"
                      "<hash-data hash=\"" + m_hash + "\">\n"
                      + "<tex type=\"" + std::to_string(TEX_2D) + "\" depth=\"" + std::to_string(m_depth) + "\"></tex>" +
                      "\n</hash-data>\n</CSEMETA>";
            if (!Settings::IsAssetsPacked())
                SaveTxtFile(asset->name_path + ".meta", hashRaw);
        }
        else {
            for(const auto& child : hashChildren) {
                const auto& type_str = child.getAttribute("type").value;
                if(!type_str.empty())
                    SetType(static_cast<Type>(std::stoi(type_str)));
                if(m_type == TEX_3D) {
                    const auto& depth_str = child.getAttribute("depth").value;
                    const auto& width_str = child.getAttribute("width").value;
                    const auto& height_str = child.getAttribute("height").value;
                    m_depth = std::stoi(depth_str);
                    m_width = std::stoi(width_str);
                    m_height = std::stoi(height_str);
                }
            }
        }
        SAFE_DELETE(root);
    }

    LoadFromMemory(reinterpret_cast<const unsigned char*>(img_str.c_str()), img_str.length());
}

void STexture::GenerateMipmap() const {
    glBindTexture(m_targetGL, m_texId);
    glGenerateMipmap(m_targetGL);
}

STexture::Type STexture::GetType() const {
    return m_type;
}

void STexture::SetType(STexture::Type type) {
    m_type = type;
    m_targetGL = GetTypeToTargetGL(type);
}

void STexture::BindEmpty(GLint location, int layout, STexture::Type type) {
    glUniform1i(location, layout);

    glActiveTexture(GL_TEXTURE0 + layout);
    glBindTexture(GetTypeToTargetGL(type), m_emptyTextureId);
}

int STexture::GetTypeToTargetGL(STexture::Type type) {
    switch (type) {
        case TEX_2D:
            return GL_TEXTURE_2D;
        case TEX_CUBE:
            return GL_TEXTURE_CUBE_MAP;
        case TEX_3D:
            return GL_TEXTURE_3D;
        default:
            return GL_TEXTURE_2D;
    }
}




