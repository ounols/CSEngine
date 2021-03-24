//
// Created by ounols on 19. 4. 18.
//

#pragma once


#include <vector>
#include "../GLProgramHandle.h"
#include "../../Object/SResource.h"

namespace CSE {

    class STexture : public SResource {
    public:
        enum TYPE {
            PNG, UNKOWN
        };
    public:
        STexture();

        virtual ~STexture();

        bool LoadFile(const char* path);

        virtual bool Load(unsigned char* data);

        bool LoadEmpty();

        bool ReloadFile(const char* path);

        bool Reload(unsigned char* data);

        unsigned int GetTextureID() const {
            return m_texId;
        }

        virtual bool InitTexture(int size);

        void Release();

        void Exterminate() override;

        virtual void Bind(GLint location, int layout);

    protected:
        virtual void Init(const AssetMgr::AssetReference* asset) override;

    protected:
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;

        unsigned int m_texId = 0;
    };

}