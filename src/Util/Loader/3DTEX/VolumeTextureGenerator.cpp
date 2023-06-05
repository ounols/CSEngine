#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include "VolumeTextureGenerator.h"
#include "../../../OGLDef.h"
#include "../../GLProgramHandle.h"
#include "../../Render/GLMeshID.h"
#include "../../Render/SMaterial.h"
#include "../../Render/ShaderUtil.h"
#include "../../Render/SEnvironmentMgr.h"
#include "../../Render/STexture.h"
#include "../../Loader/STB/stb_image_write.h"
#include "../../AssetsDef.h"

using namespace CSE;

CSE::VolumeTextureGenerator::VolumeTextureGenerator() {
    m_handle = SResource::Create<GLProgramHandle>("File:Shader/SDF/ModelGenerator.shader");
}

CSE::VolumeTextureGenerator::~VolumeTextureGenerator() {
}

void
CSE::VolumeTextureGenerator::GenerateVolumeTexture(unsigned int level, const GLMeshID& mesh, const SMaterial& material) {

    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int tex;
    int size = level * level;
    unsigned int tex2d_size = size * level;

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenRenderbuffers(1, &captureRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, tex2d_size, tex2d_size);

    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex2d_size, tex2d_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, tex2d_size, tex2d_size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    const auto& handle = m_handle;
    mat4 identity = mat4::Identity();
    mat4 view = mat4::Identity();

    glUseProgram(handle->Program);

    material.AttachElement();
    {
        auto albedo_tex = handle->UniformLocation("texture.albedo");
        if(albedo_tex != nullptr && albedo_tex->id >= 0) {
            const auto& e = material.GetElement("texture.albedo");
            if(e != nullptr) {
                STexture* tex = SResource::Get<STexture>(e->valueStr[0]);
                tex->Bind(albedo_tex->id, 0);
            }
        }

        if(handle->Uniforms.ViewMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ViewMatrix, 1, 0, view.Pointer());
    }
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // X axis Render
    Render(level, mesh, handle);

    unsigned char* data = new unsigned char[tex2d_size * tex2d_size * 4] {0};

    // X axis Render
    {
        view = mat4::RotateY(180.f);
        if(handle->Uniforms.ViewMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ViewMatrix, 1, 0, view.Pointer());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render(level, mesh, handle);

        glViewport(0, 0, tex2d_size, tex2d_size);
        auto data_y = CaptureBuffer();
        const int file_size = tex2d_size * tex2d_size * 4;
        for (int i = 0; i < file_size; i += 4) {
            unsigned char cur_alpha = data_y[i + 3];
            if(cur_alpha <= 0) continue;
            int index = i / 4;
            int x = size - index % size;
            int y = (index / tex2d_size) % size;
            int z = (int)(index / size) % level + (index / (tex2d_size * size)) * level;

            int data_index = GetIndex(level, x, y, z / 2 + size / 4);

            data_index *= 4;

            if(data[data_index] <= 0)     data[data_index] = data_y[i];
            if(data[data_index + 1] <= 0) data[data_index + 1] = data_y[i + 1];
            if(data[data_index + 2] <= 0) data[data_index + 2] = data_y[i + 2];
            if(data[data_index + 3] <= 0) data[data_index + 3] = data_y[i + 3];
        }

        delete[] data_y;
    }

    // Y axis Render
    {
        view = mat4::RotateX(90.f);
        if(handle->Uniforms.ViewMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ViewMatrix, 1, 0, view.Pointer());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render(level, mesh, handle);

        glViewport(0, 0, tex2d_size, tex2d_size);
        auto data_y = CaptureBuffer();
        const int file_size = tex2d_size * tex2d_size * 4;
        for (int i = 0; i < file_size; i += 4) {
            unsigned char cur_alpha = data_y[i + 3];
//            if(cur_alpha <= 0) continue;
            int index = i / 4;
            int x = index % size;
            int y = (index / tex2d_size) % size;
            int z = (int)(index / size) % level + (index / (tex2d_size * size)) * level;

            int data_index = GetIndex(level, x, z / 2 + size / 4, y);
            data_index *= 4;

            if(data[data_index] <= 0)     data[data_index] = data_y[i];
            if(data[data_index + 1] <= 0) data[data_index + 1] = data_y[i + 2];
            if(data[data_index + 2] <= 0) data[data_index + 2] = data_y[i + 1];
            if(data[data_index + 3] <= 0) data[data_index + 3] = data_y[i + 3];
        }

        delete[] data_y;
    }

    // Z axis Render
    {
//        view = mat4::RotateZ(180.f);
        view = mat4::RotateY(-90.f);
        if(handle->Uniforms.ViewMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ViewMatrix, 1, 0, view.Pointer());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render(level, mesh, handle);

        glViewport(0, 0, tex2d_size, tex2d_size);
        auto data_y = CaptureBuffer();
        const int file_size = tex2d_size * tex2d_size * 4;
        for (int i = 0; i < file_size; i += 4) {
            unsigned char cur_alpha = data_y[i + 3];
//            if(cur_alpha <= 0) continue;
            int index = i / 4;
            int x = index % size;
            int y = (index / tex2d_size) % size;
            int z = (int)(index / size) % level + (index / (tex2d_size * size)) * level;

            int data_index = GetIndex(level, z / 2 + size / 4, y, x);
            data_index *= 4;

            if(data[data_index] <= 0)     data[data_index] = data_y[i + 1];
            if(data[data_index + 1] <= 0) data[data_index + 1] = data_y[i + 2];
            if(data[data_index + 2] <= 0) data[data_index + 2] = data_y[i];
            if(data[data_index + 3] <= 0) data[data_index + 3] = data_y[i + 3];
        }

        delete[] data_y;
    }


    std::string save_str = CSE::AssetsPath() + "test" + ".png";
    SavePng(save_str.c_str(), tex2d_size, tex2d_size, 4, data);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    delete[] data;
}

int CSE::VolumeTextureGenerator::GetIndex(int level, int x, int y, int z) {
    int size = level * level;
    int tex2d_size = size * level;
    ivec2 offset = ivec2{ (z % level), (z / level) };
    int result = z * size * size + y * size + x;
//    int result = (offset.x * size) + (offset.y * tex2d_size * size) + x + y * tex2d_size;
    return result;
}

void CSE::VolumeTextureGenerator::Render(int level, const GLMeshID& mesh, GLProgramHandle* handle) {
    int size = level * level;
    const auto& max_distance = mesh.m_maxSize + mesh.m_maxSize;
    const auto half_distance = max_distance / 2.f;
    const auto step = max_distance / size;

    for(unsigned int i = 0; i < size; ++i) {
        glViewport((i % level) * size, (i / level) * size, size, size);
        mat4 projection = mat4::Ortho(-max_distance, max_distance,
                                      -max_distance, max_distance,
                                      -half_distance + i * step, -half_distance + (i + 1.f) * step);
//                                          -100, 100);
        if(handle->Uniforms.ProjectionMatrix >= 0)
            glUniformMatrix4fv(handle->Uniforms.ProjectionMatrix, 1, 0, projection.Pointer());

        ShaderUtil::BindAttributeToShader(*handle, mesh);

    }
}

unsigned char* CSE::VolumeTextureGenerator::CaptureBuffer() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];

    unsigned char* data = new unsigned char[width * height * 4];


    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return data;
}

int CSE::VolumeTextureGenerator::SavePng(const char* filename, int width, int height, int chennel, void* data) {
    int saved = stbi_write_png(filename, width, height, chennel, data, 0);
    return saved;
}
