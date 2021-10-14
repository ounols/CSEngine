//
// Created by ounols on 19. 8. 21.
//

#pragma once

#include <vector>
#include "MoreString.h"

namespace CSE {

    class VariableBinder {
    protected:
        typedef std::vector<std::string> Arguments;
    protected:
        VariableBinder() = default;

        virtual ~VariableBinder() = default;

    public:
        virtual void SetValue(std::string name_str, Arguments value) = 0;

        virtual std::string PrintValue() const = 0;
    };
}

#define PRINT_START(type) std::string result = std::string("<") + (type) + " type=\"" + m_classType + "\">" + '\n'
#define PRINT_END(type) result += std::string("</") + (type) + ">\n"; return result
#define PRINT_VALUE(variable, ...) {std::stringstream temp;\
result += std::string("<value name=\"") + #variable + "\">" + appandAll(temp, __VA_ARGS__) + "</value>\n";}
#define PRINT_VALUE_MAT4(variable) {\
mat4 m = variable;\
PRINT_VALUE(variable, m.x.x, ' ', m.x.y, ' ', m.x.z, ' ', m.x.w, ' ',\
m.y.x, ' ', m.y.y, ' ', m.y.z, ' ', m.y.w, ' ',\
m.z.x, ' ', m.z.y, ' ', m.z.z, ' ', m.z.w, ' ',\
m.w.x, ' ', m.w.y, ' ', m.w.z, ' ', m.w.w);}
#define PRINT_VALUE_VEC3(variable) {\
vec3 v = variable;\
PRINT_VALUE(variable, v.x, ' ', v.y, ' ', v.z);}

#define SET_VEC3(variable) vec3 vec = vec3();\
vec.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]));\
variable = vec3(vec);
#define SET_MAT4(variable) mat4 mat = mat4();\
mat.x.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));\
mat.y.Set(std::stof(value[4]), std::stof(value[5]), std::stof(value[6]), std::stof(value[7]));\
mat.z.Set(std::stof(value[8]), std::stof(value[9]), std::stof(value[10]), std::stof(value[11]));\
mat.w.Set(std::stof(value[12]), std::stof(value[13]), std::stof(value[14]), std::stof(value[15]));\
variable = mat4(mat);

