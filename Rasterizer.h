#pragma once
#include "BaseStructures.h"
#include <math.h>
//TODO: Implement more efficent drawing algorithms.
struct Camera{
    Vert pos;
    float pitch, yaw, roll;
    Vert forward, right, up;
};

class Rasterizer {
private:
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    const double zfar = 1024;
    const double znear = 0.01;
    double fov;
    int screen_width, screen_height;
    //Location of the camera and screen buffer in memory.
    Camera* cam;
    char* screenBuffer;
    void updateViewMatrix();
    Triangle processTriangle(Triangle& vertex, Matrix4x4& mesh_transform);
    
    

    
public:
    void drawPixel(int x, int y, int r = 255, int g = 255, int b = 255);
    void drawLine(int x1, int y1, int x2, int y2);
    void update();
    void fillTriangle(Triangle tri, int r, int g, int b);
    Rasterizer(Camera* cam, double fov, char* buffer, int screen_width, int screen_height);
    void processMesh(Mesh& mesh);
};