//
// Created by ounols on 19. 5. 5.
//

#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Loader/STB/stb_image_write.h"
#include "OGLDef.h"

static int saveScreenshot(const char* filename) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];

    char* data = new char[width * height * 4];


    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    int saved = stbi_write_bmp(filename, width, height, 4, data);

    delete[] data;

    return saved;
}

static int saveTexture(const char* filename) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int* x = &viewport[0];
    int* y = &viewport[1];
    int* width = &viewport[2];
    int* height = &viewport[3];

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewport[0], viewport[1], viewport[2], viewport[3], 0);

    char* data = new char[(*width)*(*height) * 4];


    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(*x, *y, *width, *height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    int saved = stbi_write_bmp(filename, *width, *height, 4, data);

    delete[] data;

    return saved;
}