//
// Created by ounols on 19. 8. 21.
//

#pragma once

#include <vector>
#include "MoreString.h"


class VariableBinder {
protected:
    typedef std::vector<std::string> Arguments;
protected:
    VariableBinder() {}

    virtual ~VariableBinder() {}

    static std::string ConvertSpaceStr(std::string str) {
        auto n = str.find(' ');
        if(n == std::string::npos) {
            return ReplaceAll(str, "$nbsp:", " ");
        }
        return ReplaceAll(str, " ", "$nbsp:");
    }
public:
    virtual void SetValue(std::string name_str, Arguments value) = 0;
    virtual std::string PrintValue() const = 0;
};


#define PRINT_START(type) std::string result = std::string("<") + type + " type=\"" + m_classType + "\">" + '\n'
#define PRINT_END(type) result += std::string("</") + type + ">\n"; return result
#define PRINT_VALUE(variable, ...) {std::stringstream temp;\
result += std::string("<value name = \"") + #variable + "\">" + appandAll(temp, __VA_ARGS__) + "</value>\n";}

