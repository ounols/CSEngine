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

#define PRINT_START(type) std::string result = std::string("<") + (type) + " type=\"" + m_classType + "\" enable=\"" + (isEnable ? "1" : "0") + "\">" + '\n'
#define PRINT_END(type) result += std::string("</") + (type) + ">\n"; return result
#define PRINT_VALUE(variable, ...) {std::stringstream temp;\
result += std::string("<value name=\"") + #variable + "\">" + appandAll(temp, __VA_ARGS__) + "</value>\n";}
#define PRINT_VALUE_SPREFAB_REF(variable) {const auto& obj = variable->GetGameObject();\
const auto& ref = dynamic_cast<SGameObjectFromSPrefab*>(variable->GetGameObject());\
std::stringstream temp;\
result += std::string("<value name=\"") + #variable + "\" ref=\"" + \
(ref == nullptr ? "0" : "1") + "\">" +\
appandAll(temp, (ref == nullptr ? obj->GetID(variable) : ref->GetRefID(variable))) + "</value>\n";\
    }
#define PRINT_VALUE_MAT4(variable) {\
mat4 m = variable;\
PRINT_VALUE(variable, m.x.x, ' ', m.x.y, ' ', m.x.z, ' ', m.x.w, ' ',\
m.y.x, ' ', m.y.y, ' ', m.y.z, ' ', m.y.w, ' ',\
m.z.x, ' ', m.z.y, ' ', m.z.z, ' ', m.z.w, ' ',\
m.w.x, ' ', m.w.y, ' ', m.w.z, ' ', m.w.w);}
#define PRINT_VALUE_VEC3(variable) {\
vec3 v = variable;\
PRINT_VALUE(variable, v.x, ' ', v.y, ' ', v.z);}
#define PRINT_VALUE_VEC4(variable) {\
vec4 v = variable;\
PRINT_VALUE(variable, v.x, ' ', v.y, ' ', v.z, ' ', v.w);}

#define SET_VEC3(variable) {vec3 vec = vec3();\
vec.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]));\
variable = std::move(vec);}
#define SET_VEC4(variable) {vec4 vec = vec4();\
vec.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));\
variable = std::move(vec);}
#define SET_MAT4(variable) {mat4 mat = mat4();\
mat.x.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]));\
mat.y.Set(std::stof(value[4]), std::stof(value[5]), std::stof(value[6]), std::stof(value[7]));\
mat.z.Set(std::stof(value[8]), std::stof(value[9]), std::stof(value[10]), std::stof(value[11]));\
mat.w.Set(std::stof(value[12]), std::stof(value[13]), std::stof(value[14]), std::stof(value[15]));\
variable = std::move(mat);}
#define SET_SPREFAB_REF(variable, component) {\
const auto& ref = dynamic_cast<SGameObjectFromSPrefab*>(variable->GetGameObject()); \
const auto& obj = variable->GetGameObject();\
if (ref != nullptr) variable = ref->GetComponentByRefHash<component>(value[0]);\
else variable = obj->GetComponentByHash<component>(value[0]);\
}

