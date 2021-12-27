#include "XMLParser.h"
#include <string>

#include "../../MoreComponentFunc.h"
#include "../../../Manager/GameObjectMgr.h"

using namespace CSE;

void XMLParser::parse(std::vector<std::string> values, void* dst, SType type) {
	if (type == SType::STRING) dst = (void*)values[0].c_str();
	else if (type == SType::BOOL) *static_cast<bool*>(dst) = parseBool(values[0].c_str());
	else if (type == SType::FLOAT) *static_cast<float*>(dst) = parseFloat(values[0].c_str());
	else if (type == SType::INT) *static_cast<int*>(dst) = parseInt(values[0].c_str());
	//    else if(type == "arr") dst = (void*)parseInt(values[0].c_str());

	else if (type == SType::RESOURCE) dst = (void*)parseResources<SResource>(values[0].c_str());
	else if (type == SType::TEXTURE) dst = (void*)parseTexture(values[0].c_str());
	else if (type == SType::MATERIAL) dst = (void*)parseMaterial(values[0].c_str());
	
	else if (type == SType::COMPONENT) dst = (void*)parseComponent(values[0].c_str());
	else if (type == SType::GAME_OBJECT) dst = (void*)parseGameObject(values[0].c_str());

	else if (type == SType::VEC2) *static_cast<vec2*>(dst) = parseVec2(values);
	else if (type == SType::VEC3) *static_cast<vec3*>(dst) = parseVec3(values);
	else if (type == SType::VEC4) *static_cast<vec4*>(dst) = parseVec4(values);
	
//	else if (type == SType::MAT2) *static_cast<mat2*>(dst) = parseMat2(values);
//	else if (type == SType::MAT3) *static_cast<mat3*>(dst) = parseMat3(values);
//	else if (type == SType::MAT4) *static_cast<mat4*>(dst) = parseMat4(values);
}

int XMLParser::parseInt(const char* value) {
	return std::stoi(value);
}

float XMLParser::parseFloat(const char* value) {
	return std::stof(value);
}

bool XMLParser::parseBool(const char* value) {
    return strncmp(value, "t", 1);
}

vec2 XMLParser::parseVec2(std::vector<std::string> values) {
	return {std::stof(values[0]), std::stof(values[1])};
}

vec3 XMLParser::parseVec3(std::vector<std::string> values) {
	return {std::stof(values[0]), std::stof(values[1]), std::stof(values[2])};
}

vec4 XMLParser::parseVec4(std::vector<std::string> values) {
	return {std::stof(values[0]), std::stof(values[1]), std::stof(values[2]), std::stof(values[3])};
}

STexture* XMLParser::parseTexture(const char* value) {
	return SResource::Create<STexture>(value);
}

SMaterial* XMLParser::parseMaterial(const char* value) {
    return SResource::Create<SMaterial>(value);
}

SComponent* XMLParser::parseComponent(const char* value) {
	SComponent* comp = CORE->GetCore(GameObjectMgr)->FindComponentByID(value);
	return comp;
}

SGameObject* XMLParser::parseGameObject(const char* value) {
	SGameObject* obj = CORE->GetCore(GameObjectMgr)->FindByID(ConvertSpaceStr(value, true));
	return obj;
}

SType XMLParser::GetType(std::string type) {
    if (type == "str") return SType::STRING;
    if (type == "bool") return SType::BOOL;
    if (type == "float") return SType::FLOAT;
    if (type == "int") return SType::INT;
//    if(type == "arr") dst = (void*)parseInt(values[0].c_str());

    if (type == "res") return SType::RESOURCE;
    if (type == "tex") return SType::TEXTURE;
    if (type == "mat") return SType::MATERIAL;

    if (type == "comp") return SType::COMPONENT;
    if (type == "gobj") return SType::GAME_OBJECT;

    if (type == "vec2") return SType::VEC2;
    if (type == "vec3") return SType::VEC3;
    if (type == "vec4") return SType::VEC4;

	if (type == "mat2") return SType::MAT2;
	if (type == "mat3") return SType::MAT3;
	if (type == "mat4") return SType::MAT4;

    return SType::UNKNOWN;
}

SType XMLParser::GetType(unsigned int type) {
    if (type == GL_BOOL) return SType::BOOL;
    if (type == GL_FLOAT) return SType::FLOAT;
    if (type == GL_INT) return SType::INT;
    //    if(type == "arr") dst = (void*)parseInt(values[0].c_str());

    if (type == GL_SAMPLER_2D) return SType::TEXTURE;


    if (type == GL_FLOAT_VEC2) return SType::VEC2;
    if (type == GL_FLOAT_VEC3) return SType::VEC3;
    if (type == GL_FLOAT_VEC4) return SType::VEC4;

    if (type == GL_FLOAT_MAT2) return SType::MAT2;
    if (type == GL_FLOAT_MAT3) return SType::MAT3;
    if (type == GL_FLOAT_MAT4) return SType::MAT4;

    return SType::UNKNOWN;
}
