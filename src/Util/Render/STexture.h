//
// Created by ounols on 19. 4. 18.
//

#pragma once


#include <vector>
#include "../../Object/SResource.h"
#include "../../OGLDef.h"

namespace CSE {

    class STexture : public SResource {
    public:
        enum Type {
            TEX_2D = 0, TEX_CUBE = 1, TEX_3D = 2
        };
    public:
        RESOURCE_DEFINE_CONSTRUCTOR(STexture);
        explicit STexture(Type type);

        ~STexture() override;

        bool LoadFile(const char* path);

        bool LoadFromMemory(const unsigned char* rawData, int length);

        virtual bool Load(unsigned char* data);

        bool ReloadFile(const char* path);

        bool Reload(unsigned char* data);

        unsigned int GetTextureID() const {
            return m_texId;
        }

        virtual bool InitTexture(int width, int height, int channel = GL_RGB, int internalFormat = GL_RGB8,
                                 int glType = GL_UNSIGNED_BYTE);

        bool InitTextureMipmap(int width, int height, int channel = GL_RGB, int internalFormat = GL_RGB8,
                                 int glType = GL_UNSIGNED_BYTE);

        virtual void SetParameteri(int targetName, int value) const;

        virtual void SetParameterfv(int targetName, float* value) const;

        void Release();

        void Exterminate() override;

        virtual void Bind(GLint location, int layout);

        static void BindEmpty(GLint location, int layout, STexture::Type type = TEX_2D);

        void GenerateMipmap() const;

        Type GetType() const;

        void SetType(Type type);

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        static void LoadEmpty();

        static int GetTypeToTargetGL(STexture::Type type);

    protected:
        Type m_type = TEX_2D;
        int m_targetGL = GL_TEXTURE_2D;
        int m_width = 0;
        int m_height = 0;
        int m_depth = 0;
        int m_channels = 0;
        int m_internalFormat = 0;
        int m_glType = GL_UNSIGNED_BYTE;

        unsigned int m_texId = 0;

    private:
        static unsigned int m_emptyTextureId;
    };

}