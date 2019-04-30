//
// Created by ounols on 19. 4. 30.
//

#include "ShaderUtil.h"



ShaderUtil::ShaderUtil() {

}

ShaderUtil::~ShaderUtil() {

}


GLProgramHandle* ShaderUtil::CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource) {
    if(vertexSource == nullptr || fragmentSource == nullptr) return nullptr;

    GLProgramHandle* gProgramhandle = nullptr;

    auto program = ShaderUtil::createProgram(vertexSource, fragmentSource);
    if (!program) {
        return nullptr;
    }

    gProgramhandle = new GLProgramHandle();
    gProgramhandle->Program = program;

    return gProgramhandle;
}


GLuint ShaderUtil::createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        return 0;
    }

    return createProgram(vertexShader, fragmentShader);
}


GLuint ShaderUtil::createProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();

    if (program) {

        //쉐이더를 attach 합니다.
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {

            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

            if (bufLength) {

                auto buf = static_cast<char*>(malloc(bufLength));

                if (buf) {

                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    std::cout << "Could not link program:\n" << buf << '\n';
                    //LOGE("Could not link program:\n%s\n", buf);
                    free(buf);

                }
            }

            glDeleteProgram(program);
            program = 0;

        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);

    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint ShaderUtil::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);

    if (shader) {

        glShaderSource(shader, 1, &pSource, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {

            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen) {

                auto buf = static_cast<char*>(malloc(infoLen));

                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    //LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    std::cout << "Could not compile shader:\n" << buf << '\n';
#ifdef WIN32
                    OutputDebugStringA(buf);
#endif
                    free(buf);
                }

                glDeleteShader(shader);
                shader = 0;
            }
        }
    }

    return shader;
}
