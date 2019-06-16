//
// Created by ounols on 19. 6. 1.
//

#pragma once

#include <vector>
#include "../GLProgramHandle.h"
#include "../../SObject.h"

class SMaterial : public SObject {
private:
    enum TYPE { NONE, INT, FLOAT, VEC3, VEC4, MAT3, MAT4 };
    struct Element {
        int id = HANDLE_NULL;
        TYPE type = NONE;
        void* value = nullptr;
    };
public:
    SMaterial();
    ~SMaterial() override;
    void Exterminate() override;

    void SetHandle(GLProgramHandle* handle);
    void AttachElement(const char* element_name, void* element);

private:
    void ReleaseElements();

    static TYPE GetType(GLenum type);

private:
    GLProgramHandle* m_handle = nullptr;
    std::vector<Element*> m_elements;
};