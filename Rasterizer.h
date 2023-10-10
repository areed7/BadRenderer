#pragma once
#include "BaseStructures.h"
#include <math.h>


struct Plane{
    Vert norm;
    float d;
};

struct ViewFrust{
    Plane nearPlane, farPlane, leftPlane, rightPlane, upPlane, downPlane;
};

struct Camera{
    Vert pos;
    float pitch, yaw, roll;
    Vert forward, right, up;

};

class Rasterizer {
private:
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    const float zfar = 1024;
    const float znear = 0.1;
    float fov;
    int screen_width, screen_height;
    //Location of the camera and screen buffer in memory.
    Camera* cam;
    char* screenBuffer;
    float*  depthBuffer;
    void updateViewMatrix();
    Triangle processTriangle(Triangle& vertex, Matrix4x4& mesh_transform);
    
    
    void updateFrustrum();
    ViewFrust frust;
    bool isPointInsidePlane(const Vert& point, Plane& frustPlane);
    bool isTriInsidePlane(Triangle& tri, Plane& frustPlane);
    bool isTriInsideFrust(Triangle& tri);
    
public:
    void drawPixel(int x, int y, int r = 255, int g = 255, int b = 255);
    void drawLine(int x1, int y1, int x2, int y2);
    void update();
    void fillTriangle(Triangle tri, int r, int g, int b);
    Rasterizer(Camera* cam, float fov, char* buffer, int screen_width, int screen_height);
    void processMesh(Mesh& mesh);
};