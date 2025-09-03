//
// Created by ounols on 19. 4. 30.
//

#pragma once

#include "../GLProgramHandle.h"
#include "GLMeshID.h"

namespace CSE {

    /** 
     * @class ShaderUtil
     *
     * @brief This class provides utility functions for shaders.
     *
     * This class provides utility functions for shaders. It can be used to create a program handle, 
     * create a program, bind variables and attributes to the shader, and bind skinning data.
     *
     */
    class ShaderUtil {
    public:
        /**
         * @brief Constructor for the ShaderUtil class.
         *
         * Constructor for the ShaderUtil class.
         */
        ShaderUtil();

        /**
         * @brief Destructor for the ShaderUtil class.
         *
         * Destructor for the ShaderUtil class.
         */
        ~ShaderUtil();

        /**
         * @brief Creates a program handle.
         *
         * Creates a program handle with the given vertex and fragment shaders.
         *
         * @param vertexSource   The vertex shader source code.
         * @param fragmentSource The fragment shader source code.
         * @param handle         The program handle to use.
         *
         * @return A pointer to the created program handle.
         */
        static GLProgramHandle* CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource,
                                                    GLProgramHandle* handle = nullptr);

        /**
         * @brief Creates a program.
         *
         * Creates a program with the given vertex and fragment shaders and the given program handle.
         *
         * @param vertexShader   The vertex shader.
         * @param fragmentShader The fragment shader.
         * @param handle         The program handle to use.
         *
         * @return The ID of the created program.
         */
        static GLuint
        createProgram(const GLchar* vertexSource, const GLchar* fragmentSource, const GLProgramHandle& handle);

        static GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, const GLProgramHandle& handle);

        /**
         * @brief Loads a shader.
         *
         * Loads a shader with the given shader type and source code and the given program handle.
         *
         * @param shaderType The type of shader to load.
         * @param pSource    The source code of the shader.
         * @param handle     The program handle to use.
         *
         * @return The ID of the loaded shader.
         */
        static GLuint loadShader(GLenum shaderType, const char* pSource, const GLProgramHandle& handle);

        /**
         * @brief Gets the important variables from the shader source code.
         *
         * Gets the important variables from the shader source code.
         *
         * @param source The shader source code.
         *
         * @return A map of variable names and types.
         */
        static std::map<std::string, std::string> GetImportantVariables(const GLchar* source);

        /**
         * @brief Binds variables to the shader.
         *
         * Binds variables to the shader.
         *
         * @param handle The program handle to use.
         */
        static void BindVariables(GLProgramHandle* handle);

        /**
         * @brief Binds the camera data to the shader.
         *
         * Binds the camera data to the shader.
         *
         * @param handle        The program handle to use.
         * @param camera        The camera transformation matrix.
         * @param cameraPosition The position of the camera.
         * @param projection    The projection matrix.
         * @param transform     The transformation matrix.
         */
        static void BindCameraToShader(const GLProgramHandle& handle, const mat4& camera, const vec3& cameraPosition,
                                       const mat4& projection, const mat4& transform);

        /**
         * @brief Binds the attributes to the shader.
         *
         * Binds the attributes to the shader.
         *
         * @param handle The program handle to use.
         * @param meshId The ID of the mesh.
         */
        static void BindAttributeToShader(const GLProgramHandle& handle, const GLMeshID& meshId);

        /**
         * @brief Binds the skinning data to the shader.
         *
         * Binds the skinning data to the shader.
         *
         * @param handle      The program handle to use.
         * @param meshId      The ID of the mesh.
         * @param jointMatrix The joint transformation matrices.
         */
        static void BindSkinningDataToShader(const GLProgramHandle& handle, const GLMeshID& meshId,
                                             const std::vector<mat4>& jointMatrix);

        static void BindAttributeToPlane();

        static void BindAttributeToCubeMap();
        
    private:
        /**
         * @brief The version of the shader define.
         *
         * The version of the shader define.
         */
        static const char *m_defineVersion;
    };

}