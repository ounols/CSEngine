//
// Created by ounols on 19. 4. 30.
//

#pragma once

#include "../GLProgramHandle.h"
#include "GLMeshID.h"

namespace CSE {

    class ShaderUtil {
    public:
        ShaderUtil();

        ~ShaderUtil();

        static GLProgramHandle* CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource,
                                                    GLProgramHandle* handle = nullptr);

        static GLuint
        createProgram(const GLchar* vertexSource, const GLchar* fragmentSource, const GLProgramHandle& handle);

        static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, const GLProgramHandle& handle);

        static GLuint loadShader(GLenum shaderType, const char *pSource, const GLProgramHandle& handle);

        static std::map<std::string, std::string> GetImportantVariables(const GLchar* source);

        static void BindVariables(GLProgramHandle* handle);

        static void BindCameraToShader(const GLProgramHandle& handle, const mat4& camera, const vec3& cameraPosition,
                                       const mat4& projection, const mat4& transform);

        static void BindAttributeToShader(const GLProgramHandle& handle, const GLMeshID& meshId);
        static void BindAttributeToPlane();

        static void BindSkinningDataToShader(const GLProgramHandle& handle, const GLMeshID& meshId,
                                             const std::vector<mat4>& jointMatrix);
    };

}