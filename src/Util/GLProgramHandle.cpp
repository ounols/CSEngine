//
// Created by ounols on 19. 5. 8.
//
#include "GLProgramHandle.h"
#include "AssetsDef.h"
#include "MoreString.h"
#include "Render/ShaderUtil.h"

GLProgramHandle::GLProgramHandle() : Program(HANDLE_NULL) {
    SetUndestroyable(true);

}

GLProgramHandle::~GLProgramHandle() {

}

void GLProgramHandle::Exterminate() {
    glDeleteProgram(Program);

    for(auto element_pair : AttributesList) {
        Element* element = element_pair.second;
        SAFE_DELETE(element);
    }

    for(auto element_pair : UniformsList) {
        Element* element = element_pair.second;
        SAFE_DELETE(element);
    }
}

void GLProgramHandle::SetAttribVec3(std::string location, vec3& value) {
    glUseProgram(Program);
    auto iter = AttributesList.find(location);
    if(iter != AttributesList.end()) {
        glVertexAttrib3fv(iter->second->id, value.Pointer());
        return;
    }
    glVertexAttrib3fv(glGetAttribLocation(Program, location.c_str()), value.Pointer());
}

void GLProgramHandle::SetAttribVec4(std::string location, vec4& value) {
    glUseProgram(Program);
    auto iter = AttributesList.find(location);
    if(iter != AttributesList.end()) {
        glVertexAttrib4fv(iter->second->id, value.Pointer());
        return;
    }
    glVertexAttrib4fv(glGetAttribLocation(Program, location.c_str()), value.Pointer());
}

void GLProgramHandle::SetUniformInt(std::string location, int value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniform1i(iter->second->id, value);
        return;
    }
    glUniform1i(glGetUniformLocation(Program, location.c_str()), value);
}

void GLProgramHandle::SetUniformFloat(std::string location, float value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniform1f(iter->second->id, value);
        return;
    }
    glUniform1f(glGetUniformLocation(Program, location.c_str()), value);
}

void GLProgramHandle::SetUniformMat4(std::string location, mat4& value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniformMatrix4fv(iter->second->id, 1, 0, value.Pointer());
        return;
    }
    glUniformMatrix4fv(glGetUniformLocation(Program, location.c_str()), 1, 0, value.Pointer());
}

void GLProgramHandle::SetUniformMat3(std::string location, mat3& value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniformMatrix3fv(iter->second->id, 1, 0, value.Pointer());
        return;
    }
    glUniformMatrix3fv(glGetUniformLocation(Program, location.c_str()), 1, 0, value.Pointer());
}

void GLProgramHandle::SetAttributesList(std::map<std::string, std::string>& vert,
                                        std::map<std::string, std::string>& frag) {
    GLint count = 0;
    GLsizei length = 0;
    GLint size = 0;
    GLenum type;

    glGetProgramiv(Program, GL_ACTIVE_ATTRIBUTES, &count);

    for (int i = 0; i < count; i++) {
        GLchar name[32];
        glGetActiveAttrib(Program, (GLuint) i, 32, &length, &size, &type, name);

        std::string name_str = name;
        std::string imp_name = getImplementName(vert, name_str);
        auto& target = vert;

        if(imp_name.empty()) {
            imp_name = getImplementName(frag, name_str);
            if(imp_name.empty()) continue;

            target = frag;
        }

        int id = glGetAttribLocation(Program, target.find(imp_name)->second.c_str());

        Element* element = new Element;
        element->type = type;
        element->id = id;

        AttributesList[imp_name] = element;
    }
}

void GLProgramHandle::SetUniformsList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag) {
    GLint count = 0;
    GLsizei length = 0;
    GLint size = 0;
    GLenum type;

    glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &count);

    for (int i = 0; i < count; i++) {
        GLchar name[64];
        glGetActiveUniform(Program, (GLuint) i, 64, &length, &size, &type, name);

        std::string name_str = name;

        std::string imp_name = getImplementName(vert, name_str);
        auto& target = vert;

        if(imp_name.empty()) {
            imp_name = getImplementName(frag, name_str);
            if(imp_name.empty()) continue;

            target = frag;
        }

        int id = glGetUniformLocation(Program, target.find(imp_name)->second.c_str());

        Element* element = new Element;
        element->type = type;
        element->id = id;

        UniformsList[imp_name] = element;
    }
}

GLProgramHandle::GLElementList GLProgramHandle::GetAttributesList() {
    return AttributesList;
}

GLProgramHandle::GLElementList GLProgramHandle::GetUniformsList() {
    return UniformsList;
}


void GLProgramHandle::SaveShader(std::string path) {
    return;
}

std::string GLProgramHandle::getImplementName(std::map <std::string, std::string>& list, std::string name) {

    for(auto it = list.begin(); it != list.end(); it++) {
        if(name.find(it->second) != std::string::npos) {
            return it->first;
        }
    }

    return "";
}

void GLProgramHandle::Init(const AssetMgr::AssetReference* asset) {
    //컴바인 쉐이더 로드
    std::string shader_combine = CSE::OpenAssetsTxtFile(asset->path);

    auto shader_combine_vector = split(shader_combine, ',');

    if(shader_combine_vector.size() < 2) return;
    m_vertShaderName = trim(shader_combine_vector.at(0));
    m_fragShaderName = trim(shader_combine_vector.at(1));

    //데이터 바이딩
    auto vert_asset = ResMgr::getInstance()->GetAssetReference(m_vertShaderName);
    auto frag_asset = ResMgr::getInstance()->GetAssetReference(m_fragShaderName);

    std::string vert_str = CSE::OpenAssetsTxtFile(vert_asset->path);
    std::string frag_str = CSE::OpenAssetsTxtFile(frag_asset->path);

    if(vert_str.empty() || frag_str.empty()) return;

    if(ShaderUtil::CreateProgramHandle(vert_str.c_str(), frag_str.c_str(), this) == nullptr) {
        return;
    }

    return;
}