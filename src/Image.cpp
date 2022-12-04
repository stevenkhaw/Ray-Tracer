#include "Image.h"

void Image::init() {
    this->pixels = std::vector<glm::vec3>(this->width * this->height);
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tbo);
    testAddColor();
}

void Image::draw() {
    glBindTexture(GL_TEXTURE_2D, tbo);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, // load texture
        0, GL_RGB, GL_FLOAT, &pixels[0][0]);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, tbo, 0); // attach texture and the read frame

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // if not already so

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT,
        GL_NEAREST); // copy framebuffer from read to write
}

void Image::testAddColor() {
    glm::vec3 GL_RBG_RED = glm::vec3(1.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < width * height; i++) {
        this->pixels[i] = GL_RBG_RED;
    }
}