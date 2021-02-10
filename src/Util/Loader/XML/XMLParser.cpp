#include "XMLParser.h"
#include <string>

#include "../../MoreComponentFunc.h"
#include "../../../Manager/GameObjectMgr.h"

using namespace CSE;

void XMLParser::parse(std::vector<std::string> values, void* dst, const char* type) {
	if (type == "str") dst = (void*)values[0].c_str();
	else if (type == "bool") dst = (void*)parseBool(values[0].c_str());
	else if (type == "float") *static_cast<float*>(dst) = parseFloat(values[0].c_str());
	else if (type == "int") dst = (void*)parseInt(values[0].c_str());
	//    else if(type == "arr") dst = (void*)parseInt(values[0].c_str());

	else if (type == "res") dst = (void*)parseResources<SResource>(values[0].c_str());
	else if (type == "tex") dst = (void*)parseTexture(values[0].c_str());
	else if (type == "mat") dst = (void*)parseMaterial(values[0].c_str());
	
	else if (type == "comp") dst = (void*)parseComponent(values[0].c_str());
	else if (type == "gobj") dst = (void*)parseGameObject(values[0].c_str());
	
	else if (type == "vec2") *static_cast<vec2*>(dst) = parseVec2(values);
	else if (type == "vec3") *static_cast<vec3*>(dst) = parseVec3(values);
	else if (type == "vec4") *static_cast<vec4*>(dst) = parseVec4(values);
	
	else if (type == "mat2") *static_cast<mat2*>(dst) = parseMat2(values);
	else if (type == "mat3") *static_cast<mat3*>(dst) = parseMat3(values);
	else if (type == "mat4") *static_cast<mat4*>(dst) = parseMat4(values);
}

int XMLParser::parseInt(const char* value) {
	return std::stoi(value);
}

float XMLParser::parseFloat(const char* value) {
	return std::stof(value);
}

bool XMLParser::parseBool(const char* value) {
	return value == "t";
}

vec2 XMLParser::parseVec2(std::vector<std::string> values) {
	return vec2(std::stof(values[0]), std::stof(values[1]));
}

vec3 XMLParser::parseVec3(std::vector<std::string> values) {
	return vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
}

vec4 XMLParser::parseVec4(std::vector<std::string> values) {
	return vec4(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]), std::stof(values[3]));
}

STexture* XMLParser::parseTexture(const char* value) {
	return parseResources<STexture>(value);
}

SMaterial* XMLParser::parseMaterial(const char* value) {
	return parseResources<SMaterial>(value);
}

SComponent* XMLParser::parseComponent(const char* value) {
	SComponent* comp = GameObjectMgr::getInstance()->FindComponentByID(value);
	return comp;
}

SGameObject* XMLParser::parseGameObject(const char* value) {
	SGameObject* obj = GameObjectMgr::getInstance()->FindByID(ConvertSpaceStr(value, true));
	return obj;
}
