//
// Created by ounols on 19. 5. 8.
//
#include "GLProgramHandle.h"

GLProgramHandle::GLProgramHandle() : Program(HANDLE_NULL) {
    ResMgr::getInstance()->Register<ShaderProgramContainer, GLProgramHandle>(this);
    SetUndestroyable(true);

}

GLProgramHandle::~GLProgramHandle() {

}

void GLProgramHandle::Exterminate() {
    glDeleteProgram(Program);
}

void GLProgramHandle::SetAttribVec3(std::string location, vec3& value) {
    glUseProgram(Program);
    auto iter = AttributesList.find(location);
    if(iter != AttributesList.end()) {
        glVertexAttrib3fv(iter->second, value.Pointer());
        return;
    }
    glVertexAttrib3fv(glGetAttribLocation(Program, location.c_str()), value.Pointer());
}

void GLProgramHandle::SetAttribVec4(std::string location, vec4& value) {
    glUseProgram(Program);
    auto iter = AttributesList.find(location);
    if(iter != AttributesList.end()) {
        glVertexAttrib4fv(iter->second, value.Pointer());
        return;
    }
    glVertexAttrib4fv(glGetAttribLocation(Program, location.c_str()), value.Pointer());
}

void GLProgramHandle::SetUniformInt(std::string location, int value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniform1i(iter->second, value);
        return;
    }
    glUniform1i(glGetUniformLocation(Program, location.c_str()), value);
}

void GLProgramHandle::SetUniformFloat(std::string location, float value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniform1f(iter->second, value);
        return;
    }
    glUniform1f(glGetUniformLocation(Program, location.c_str()), value);
}

void GLProgramHandle::SetUniformMat4(std::string location, mat4& value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniformMatrix4fv(iter->second, 1, 0, value.Pointer());
        return;
    }
    glUniformMatrix4fv(glGetUniformLocation(Program, location.c_str()), 1, 0, value.Pointer());
}

void GLProgramHandle::SetUniformMat3(std::string location, mat3& value) {
    glUseProgram(Program);
    auto iter = UniformsList.find(location);
    if(iter != UniformsList.end()) {
        glUniformMatrix3fv(iter->second, 1, 0, value.Pointer());
        return;
    }
    glUniformMatrix3fv(glGetUniformLocation(Program, location.c_str()), 1, 0, value.Pointer());
}

void GLProgramHandle::GetAttributesList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag) {
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

        AttributesList[imp_name] = id;
    }
}

void GLProgramHandle::GetUniformsList(std::map<std::string, std::string>& vert, std::map<std::string, std::string>& frag) {
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

        UniformsList[imp_name] = id;
    }
}

std::string GLProgramHandle::getImplementName(std::map <std::string, std::string>& list, std::string name) {

    for(auto it = list.begin(); it != list.end(); it++) {
        if(name.find(it->second) != std::string::npos) {
            return it->first;
        }
    }

    return "";
}