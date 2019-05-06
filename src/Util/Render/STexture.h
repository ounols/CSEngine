//
// Created by ounols on 19. 4. 18.
//

#pragma once


#include <vector>
#include "../GLProgramHandle.h"
#include "../../SObject.h"

class STexture : public SObject {
public:
    enum TYPE { PNG, UNKOWN };
public:
    STexture();
    virtual ~STexture();

    bool LoadFile(const char* path);
    virtual bool Load(unsigned char* data);
    bool LoadEmpty();
    bool ReloadFile(const char* path);
    bool Reload(unsigned char* data);

    unsigned int GetID() const {
        return m_id;
    }

    void Release();
    void Exterminate() override;

    virtual void Bind(const GLProgramHandle* handle, int layout = 0);

protected:
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;

    unsigned int m_id = 0;
};


