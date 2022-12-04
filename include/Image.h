#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>
#define GLM_FORCE_RADIANS
#include <math.h>
#include <glm/glm.hpp>

class Image {
public:
    int width;
    int height;
    std::vector<glm::vec3> pixels; // RGB colors

    void init();
    void draw();
    void testAddColor();

    Image(int width, int height) {
        this->width = width;
        this->height = height;
    }
private:
    unsigned int fbo; // Frame buffer
    unsigned int tbo; // Texture buffer
};