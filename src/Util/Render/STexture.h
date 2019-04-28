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
    ~STexture();

    bool Load(const char* path, TYPE type = PNG);
    bool Load(std::vector<unsigned char> data, TYPE type);
    bool LoadEmpty();
    bool Reload(const char* path, TYPE type = PNG);
    bool Reload(std::vector<unsigned char> data, TYPE type);

    void Release();
    void Exterminate() override;

    void Bind(const GLProgramHandle* handle);

private:
    unsigned int m_width = 0;
    unsigned int m_height = 0;

    unsigned int m_id = 0;
};


