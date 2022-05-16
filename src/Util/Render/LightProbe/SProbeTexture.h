#pragma once

namespace CSE {
    /**
     * 일반적인 STexture와 다르게 경량화 된 특수한 오브젝트
     */
    class SProbeTexture {
    public:
        SProbeTexture();
        ~SProbeTexture();

    public:
        bool LoadFromMemory(const unsigned char* rawData, int length);
        void InitTexture(int width = 4);

        unsigned int GetTextureID() const {
            return m_texId;
        }

    private:
        unsigned short m_width = 4;
        unsigned int m_texId = 0;
    };
}