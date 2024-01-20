#include "XMLParser.h"
#include <string>

#include "../../MoreComponentFunc.h"
#include "../../../Manager/GameObjectMgr.h"
#include "../../Render/SFrameBuffer.h"
#include "../../Render/SMaterial.h"
#include "../../Render/STexture.h"

using namespace CSE;

void XMLParser::parse(std::vector<std::string> values, void* dst, SType type) {
    if (type == SType::STRING) dst = (void*) values[0].c_str();
    else if (type == SType::BOOL) *static_cast<bool*>(dst) = parseBool(values[0].c_str());
    else if (type == SType::FLOAT) *static_cast<float*>(dst) = parseFloat(values[0].c_str());
    else if (type == SType::INT) *static_cast<int*>(dst) = parseInt(values[0].c_str());
        //    else if(type == "arr") dst = (void*)parseInt(values[0].c_str());

    else if (type == SType::RESOURCE) dst = (void*) parseResources<SResource>(values[0].c_str());
    else if (type == SType::MATERIAL) dst = (void*) parseMaterial(values[0].c_str());
    else if (type == SType::TEXTURE) {
        dst = (void*) parseTexture(values[0].c_str());
        if (dst == nullptr) dst = (void*) parseFrameBuffer(values[0].c_str());
    } else if (type == SType::COMPONENT) dst = (void*) parseComponent(values[0].c_str());
    else if (type == SType::GAME_OBJECT) dst = (void*) parseGameObject(values[0].c_str());

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
    const auto& asset = CORE->GetCore(ResMgr)->GetAssetReference(value);
    if (asset == nullptr || asset->type == AssetMgr::FRAMEBUFFER) {
        const auto value_split = split(value, '?');
        if (value_split.size() <= 1)
            return SResource::Create<STexture>(value);

        const auto& frameBuffer = SResource::Create<SFrameBuffer>(value_split[0]);
        return frameBuffer->GetTexture(value);
    }
    return SResource::Create<STexture>(value);
}

SFrameBuffer* XMLParser::parseFrameBuffer(const char* value) {
    std::string value_result = value;
    const auto split_index = value_result.rfind('?');
    if (split_index != std::string::npos)
        value_result = value_result.substr(0, split_index - 1);

    const auto& asset = CORE->GetCore(ResMgr)->GetAssetReference(value_result);
    if (asset->type != AssetMgr::FRAMEBUFFER) {
        return nullptr;
    }
    return SResource::Create<SFrameBuffer>(asset);
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
    static const std::unordered_map<std::string, SType> typeMap = {
            {"str", SType::STRING},
            {"bool", SType::BOOL},
            {"float", SType::FLOAT},
            {"int", SType::INT},
            {"res", SType::RESOURCE},
            {"tex", SType::TEXTURE},
            {"mat", SType::MATERIAL},
            {"comp", SType::COMPONENT},
            {"gobj", SType::GAME_OBJECT},
            {"vec2", SType::VEC2},
            {"vec3", SType::VEC3},
            {"vec4", SType::VEC4},
            {"mat2", SType::MAT2},
            {"mat3", SType::MAT3},
            {"mat4", SType::MAT4}
    };

    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    } else {
        return SType::UNKNOWN;
    }
}

SType XMLParser::GetType(unsigned int type) {
    switch (type) {
        case GL_BOOL: return SType::BOOL;
        case GL_FLOAT: return SType::FLOAT;
        case GL_INT: return SType::INT;
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_2D: return SType::TEXTURE;
        case GL_FLOAT_VEC2: return SType::VEC2;
        case GL_FLOAT_VEC3: return SType::VEC3;
        case GL_FLOAT_VEC4: return SType::VEC4;
        case GL_FLOAT_MAT2: return SType::MAT2;
        case GL_FLOAT_MAT3: return SType::MAT3;
        case GL_FLOAT_MAT4: return SType::MAT4;
        default: return SType::UNKNOWN;
    }
}

std::string XMLParser::ToString(SType type) {
    switch (type) {
        case SType::STRING: return "str";
        case SType::BOOL: return "bool";
        case SType::FLOAT: return "float";
        case SType::INT: return "int";
        case SType::RESOURCE: return "res";
        case SType::TEXTURE: return "tex";
        case SType::MATERIAL: return "mat";
        case SType::COMPONENT: return "comp";
        case SType::GAME_OBJECT: return "gobj";
        case SType::VEC2: return "vec2";
        case SType::VEC3: return "vec3";
        case SType::VEC4: return "vec4";
        case SType::MAT2: return "mat2";
        case SType::MAT3: return "mat3";
        case SType::MAT4: return "mat4";
        default: return "str";
    }
}