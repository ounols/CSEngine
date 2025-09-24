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
        /**
         * Used to get serialized values.
         * All classes that inherit from VariableBinder will unconditionally call this function first during initialization.
         * @param name_str Name of values
         * @param value Serialized values
         */
        virtual void SetValue(const std::string& name_str, const Arguments& value) = 0;

        virtual std::string PrintValue() const = 0;
    };
}

#define PRINT_START(type) std::string result; \
result.reserve(4096); \
char buffer[512]; \
snprintf(buffer, sizeof(buffer), "<%s type=\"%s\" enable=\"%c\">\n", (type), GetClassType(), (isEnable ? '1' : '0')); \
result = buffer

#define PRINT_END(type) { \
char buffer[256]; \
snprintf(buffer, sizeof(buffer), "</%s>\n", (type)); \
result += buffer; \
return result; \
}

#define PRINT_VALUE(type, variable, ...) { \
char buffer[1024]; \
std::stringstream temp; \
auto value_str = appandAll(temp, __VA_ARGS__); \
snprintf(buffer, sizeof(buffer), "<value name=\"%s\" t=\"%s\">%s</value>\n", #variable, type, value_str.c_str()); \
result += buffer; \
}

#define PRINT_COMP_NAME(name, variable) { \
char buffer[1024]; \
auto hash_str = ConvertSpaceStr(variable->GetHash()); \
snprintf(buffer, sizeof(buffer), "<value name=\"%s\" t=\"comp\" d=\"%s\">%s</value>\n", #name, variable->GetClassType(), hash_str.c_str()); \
result += buffer; \
}

#define PRINT_RES_NAME(name, variable) { \
char buffer[1024]; \
auto hash_str = ConvertSpaceStr(variable->GetHash()); \
snprintf(buffer, sizeof(buffer), "<value name=\"%s\" t=\"res\" d=\"%s\">%s</value>\n", #name, variable->GetClassType(), hash_str.c_str()); \
result += buffer; \
}

#define PRINT_COMP(variable) PRINT_COMP_NAME(variable, variable)
#define PRINT_RES(variable) PRINT_RES_NAME(variable, variable)

#define PRINT_VALUE_SPREFAB_REF(variable) { \
char buffer[1024]; \
const auto& obj = variable->GetGameObject(); \
const auto& ref = static_cast<SGameObjectFromSPrefab*>(variable->GetGameObject()); \
std::stringstream temp; \
auto id_str = appandAll(temp, (ref == nullptr ? obj->GetID(variable) : ref->GetRefID(variable))); \
snprintf(buffer, sizeof(buffer), "<value name=\"%s\" ref=\"%c\" t=\"prefab\">%s</value>\n", #variable, (ref == nullptr ? '0' : '1'), id_str.c_str()); \
result += buffer; \
}
#define PRINT_VALUE_MAT4(variable) {\
mat4 m = variable;\
PRINT_VALUE("mat4", variable, m.x.x, ' ', m.x.y, ' ', m.x.z, ' ', m.x.w, ' ',\
m.y.x, ' ', m.y.y, ' ', m.y.z, ' ', m.y.w, ' ',\
m.z.x, ' ', m.z.y, ' ', m.z.z, ' ', m.z.w, ' ',\
m.w.x, ' ', m.w.y, ' ', m.w.z, ' ', m.w.w);}
#define PRINT_VALUE_VEC3(variable) {\
vec3 v = variable;\
PRINT_VALUE("vec3", variable, v.x, ' ', v.y, ' ', v.z);}
#define PRINT_VALUE_VEC4(variable) {\
vec4 v = variable;\
PRINT_VALUE("vec4", variable, v.x, ' ', v.y, ' ', v.z, ' ', v.w);}

#define PRINT_VALUE_COLOR3(variable) {\
vec3 v = variable;\
PRINT_VALUE("col3", variable, v.x, ' ', v.y, ' ', v.z);}
#define PRINT_VALUE_COLOR4(variable) {\
vec4 v = variable;\
PRINT_VALUE("col4", variable, v.x, ' ', v.y, ' ', v.z, ' ', v.w);}

#define SET_VEC3(variable) variable.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]))
#define SET_VEC4(variable) variable.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3]))
#define SET_MAT4(variable) { \
variable.x.Set(std::stof(value[0]), std::stof(value[1]), std::stof(value[2]), std::stof(value[3])); \
variable.y.Set(std::stof(value[4]), std::stof(value[5]), std::stof(value[6]), std::stof(value[7])); \
variable.z.Set(std::stof(value[8]), std::stof(value[9]), std::stof(value[10]), std::stof(value[11])); \
variable.w.Set(std::stof(value[12]), std::stof(value[13]), std::stof(value[14]), std::stof(value[15])); \
}
#define SET_SPREFAB_REF(variable, component) {\
const auto& ref = dynamic_cast<SGameObjectFromSPrefab*>(variable->GetGameObject()); \
const auto& obj = variable->GetGameObject();\
if (ref != nullptr) variable = ref->GetComponentByRefHash<component>(value[0]);\
else variable = obj->GetComponentByHash<component>(value[0]);\
}

