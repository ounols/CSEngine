//
// Created by ounols on 19. 6. 10.
//

#pragma once

#include <string>
#include "../SObject.h"

class SResource : public SObject {
public:
    SResource();
    virtual ~SResource();

    void SetName(std::string name);

    const char* GetName() const {
        return m_name.c_str();
    }

private:
    std::string m_name;

};



