#include "XMLParser.h"
#include <string>

using namespace CSE;

int XMLParser::parseInt(const char* value)
{
    return std::stoi(value);
}

float XMLParser::parseFloat(const char* value)
{
    return std::stof(value);
}

bool XMLParser::parseBool(const char* value)
{
    return value == "t";
}

vec2 XMLParser::parseVec2(std::vector<std::string> values)
{
    return vec2(std::stof(values[0]), std::stof(values[1]));
}

vec3 XMLParser::parseVec3(std::vector<std::string> values)
{
    return vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
}

vec4 XMLParser::parseVec4(std::vector<std::string> values)
{
    return vec4(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]), std::stof(values[3]));
}
