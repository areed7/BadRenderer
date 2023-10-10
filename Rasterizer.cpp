#include "Rasterizer.h"
#include <cmath>
#include <iostream>
#include <cstring>
#include <math.h>
#define _USE_MATH_DEFINES

/* 
 * fov : FOV in degrees
 * screen_width : Width rendering area/screen (Char buffer)
 * screen_height : Height of the rendering area/screen (Char buffer)
 * 
*/
Rasterizer::Rasterizer(Camera* cam, double fov, char* buffer, int screen_width, int screen_height){
    this->fov = fov;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    this->cam = cam;
    this->screenBuffer = buffer;
    depthBuffer = (float* ) malloc(screen_width*screen_height*sizeof(float));
    for( int i = 0; i < screen_width*screen_height; i++){
        *(depthBuffer + i) = (float)zfar;
    }
    float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);
    //Only needs to be processed once.
    projectionMatrix.data[0][0] = (screen_width/screen_height)*fovRad;
    projectionMatrix.data[1][1] = fovRad;
    projectionMatrix.data[2][2] = (zfar / (zfar - znear));
    projectionMatrix.data[3][3] = (-zfar * znear) / (zfar - znear);
    projectionMatrix.data[3][2] = 1.0;
    projectionMatrix.data[3][3] = 0.0;

    updateViewMatrix();
    updateFrustrum();
}

bool Rasterizer::isPointInsidePlane(const Vert& point, Plane& frustPlane) {
    
    return frustPlane.norm.dot(point) + frustPlane.d < 0;
}

bool Rasterizer::isTriInsidePlane(Triangle& tri, Plane& frustPlane){
    bool A = isPointInsidePlane(tri.a, frustPlane);
    bool B = isPointInsidePlane(tri.b, frustPlane);
    bool C = isPointInsidePlane(tri.c, frustPlane);
    return A||B||C;
}

bool Rasterizer::isTriInsideFrust(Triangle& tri){
    return isTriInsidePlane(tri, frust.rightPlane) &&
        isTriInsidePlane(tri, frust.leftPlane) &&
        isTriInsidePlane(tri, frust.upPlane) &&
        isTriInsidePlane(tri, frust.downPlane) &&
        isTriInsidePlane(tri, frust.farPlane) &&
        isTriInsidePlane(tri, frust.nearPlane);
        
}

void Rasterizer::updateFrustrum(){
    
    double fovRad = fov * M_PI / 180;
    double cosPitch  = cos(cam->pitch);
    double sinPitch  = sin(cam->pitch);
    double cosPitchP = cos(cam->pitch + fovRad/2);
    double sinPitchP = sin(cam->pitch + fovRad/2);
    double cosPitchN = cos(cam->pitch - fovRad/2);
    double sinPitchN = sin(cam->pitch - fovRad/2);
    double cosYaw = cos(cam->yaw);
    double sinYaw = sin(cam->yaw);
    double cosYawP = cos(cam->yaw + fovRad/2);
    double cosYawN = cos(cam->yaw - fovRad/2);
    double sinYawP = sin(cam->yaw + fovRad/2);
    double sinYawN = sin(cam->yaw - fovRad/2);

    frust.rightPlane.norm = cam->up.cross(Vert(sinYawP, 0, cosYawP, 0)).normalize();
    frust.rightPlane.d = -cam->pos.dot(frust.rightPlane.norm);

    frust.leftPlane.norm = Vert(sinYawN, 0, cosYawN, 0).cross(cam->up).normalize();
    frust.leftPlane.d = -cam->pos.dot(frust.leftPlane.norm);
    
    frust.upPlane.norm = (Vert(cosPitchN * sinYaw, -sinPitchN, cosPitchN * cosYaw, 0).cross(cam->right)).normalize();
    frust.upPlane.d = -cam->pos.dot(frust.upPlane.norm);

    frust.downPlane.norm = (cam->right.cross(Vert(cosPitchP * sinYaw, -sinPitchP, cosPitchP * cosYaw, 0))).normalize();
    frust.downPlane.d = -cam->pos.dot(frust.downPlane.norm);

    frust.nearPlane.norm = cam->forward*-1;
    frust.nearPlane.d    = -frust.nearPlane.norm.dot(cam->pos) - znear*frust.nearPlane.norm.length();
    frust.farPlane.norm  = cam->forward;
    frust.farPlane.d     = -frust.farPlane.norm.dot(cam->pos) - zfar*frust.farPlane.norm.length();
}
void Rasterizer::updateViewMatrix(){
    double cosPitch = cos(cam->pitch);
    double sinPitch = sin(cam->pitch);
    double cosYaw = cos(cam->yaw);
    double sinYaw = sin(cam->yaw);

    cam->forward = Vert(cosPitch * sinYaw, -sinPitch,cosPitch*cosYaw,0);
    //cam->forward = Vert(cosPitch,-sinPitch,0,0);
    cam->forward = cam->forward.normalize();
    
    Vert a = Vert(0,1,0,0)*(Vert(0,1,0,0).dot(cam->forward));
    cam->up= Vert(0,1,0,0) - a;
    cam->up = cam->up.normalize();
    
    cam->right      = cam->up.cross(cam->forward);
    cam->right      = cam->right.normalize();

    cam->up = cam->forward.cross(cam->right);
    cam->up = cam->up.normalize();
    viewMatrix.data[0][0] = cam->right.x;
    viewMatrix.data[0][1] = cam->right.y;
    viewMatrix.data[0][2] = cam->right.z;
    viewMatrix.data[0][3] = -cam->right.dot(cam->pos);

    viewMatrix.data[1][0] = cam->up.x;
    viewMatrix.data[1][1] = cam->up.y;
    viewMatrix.data[1][2] = cam->up.z;
    viewMatrix.data[1][3] = -cam->up.dot(cam->pos);

    viewMatrix.data[2][0] = cam->forward.x;
    viewMatrix.data[2][1] = cam->forward.y;
    viewMatrix.data[2][2] = cam->forward.z;
    viewMatrix.data[2][3] = -cam->forward.dot(cam->pos);

    viewMatrix.data[3][0] = 0;
    viewMatrix.data[3][1] = 0;
    viewMatrix.data[3][2] = 0;
    viewMatrix.data[3][3] = 1;
}

void Rasterizer::update(){
    //Clear buffer;
    for( int i = 0; i < screen_width*screen_height; i++){
        *(depthBuffer + i) = (float)zfar;
    }
    updateViewMatrix();
    updateFrustrum();
}

void Rasterizer::drawPixel(int x, int y, int r, int g, int b){

    if( x >= screen_width || y >= screen_height || x < 0 || y < 0){
        return;
    }
    *(screenBuffer + (y*screen_width + x)*3 + 0) = r;
    *(screenBuffer + (y*screen_width + x)*3 + 1) = g;
    *(screenBuffer + (y*screen_width + x)*3 + 2) = b;
}

//Bresenham's line algorithm
void Rasterizer::drawLine(int x1, int y1, int x2, int y2) {
    //std::cout << "DrawLine" << std::endl;
    // Check if the line is vertical
    if (x2 == x1) {
        int yStart = std::min(y1, y2);
        int yEnd = std::max(y1, y2);
        for (int y = std::max(yStart,0); y <= std::min(yEnd, screen_width-1); ++y) {
            drawPixel(x1, y, 255, 0, 0);
        }
        return;
    }

    // Calculate slope
    double slope = static_cast<double>(y2 - y1) / (x2 - x1);

    // Check if the absolute slope is <= 1 (i.e., not too steep)
    if (std::abs(slope) <= 1.0) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        for (int x = std::max(x1,0); x <= std::min(x2,screen_width-1); ++x) {
            int y = static_cast<int>(y1 + slope * (x - x1));
            drawPixel(x, y, 255, 0, 0);
        }
    } else {
        // Handle steep slopes (loop over y values)
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        for (int y = std::max(0, y1); y <= std::min(y2, screen_height-1); ++y) {
            int x = static_cast<int>(x1 + (y - y1) / slope);
            drawPixel(x, y, 255, 0, 0);
        }
    }
    //std::cout << "End DrawLine" << std::endl;
}

/*
 * Takes a triangle in the world, and returns the corresponding triangle on the screen.
*/
Triangle Rasterizer::processTriangle(Triangle& vertex, Matrix4x4& mesh_transform){
    Vert t1, t2, t3;
    t1 = mesh_transform*vertex.a;
    t2 = mesh_transform*vertex.b;
    t3 = mesh_transform*vertex.c;
    Triangle triTest;
    triTest.a = t1;
    triTest.b = t2;
    triTest.c = t3;
    triTest.norm = Vert(0,0,0,0);
    if(isTriInsideFrust(triTest)){
        Vert v1, v2, v3;
        v1 = viewMatrix*t1;
        v2 = viewMatrix*t2;
        v3 = viewMatrix*t3;

        Vert norm, l1, l2;
        l1 = v2-v1;
        l2 = v3-v1;
        norm = l1.cross(l2);
        norm = norm.normalize();
        
        double dot = norm.dot(v1);
        if(dot < 0.0)
        {
            Vert s1 = projectionMatrix*v1;
            s1 = s1/s1.w;
            s1.x = ((s1.x+1.0)/2.0)*screen_width;
            s1.y = ((s1.y+1.0)/2.0)*screen_height;
            Vert s2 = projectionMatrix*v2;
            s2 = s2/s2.w;
            s2.x = ((s2.x+1.0)/2.0)*screen_width;
            s2.y = ((s2.y+1.0)/2.0)*screen_height;
            Vert s3 = projectionMatrix*v3;
            s3 = s3/s3.w;
            s3.x = ((s3.x+1.0)/2.0)*screen_width;
            s3.y = ((s3.y+1.0)/2.0)*screen_height;
            Triangle screenTriangle;
            screenTriangle.a = s1;
            screenTriangle.b = s2;
            screenTriangle.c = s3;
            screenTriangle.norm = norm;
            return screenTriangle;
        }
    }
}


//Barycentric Algorithm
void Rasterizer::fillTriangle(Triangle tri, int r, int g, int b) {
    int maxX = std::min((double)(screen_width - 1), std::max(tri.a.x, std::max(tri.b.x, tri.c.x)));
    int minX = std::max(0.0, std::min(tri.a.x, std::min(tri.b.x, tri.c.x)));
    int maxY = std::min((double)(screen_height - 1), std::max(tri.a.y, std::max(tri.b.y, tri.c.y)));
    int minY = std::max(0.0, std::min(tri.a.y, std::min(tri.b.y, tri.c.y)));

    Vert2D vs1(tri.b.x - tri.a.x, tri.b.y - tri.a.y);
    Vert2D vs2(tri.c.x - tri.a.x, tri.c.y - tri.a.y);
    float crossVs = vs1.cross(vs2);

    for (int x = std::max(minX,0); x <= std::min(maxX,screen_width-1); x++) {
        for (int y = std::max(minY,0); y <= std::min(maxY,screen_height-1); y++) {
            int idx = y * screen_width + x;
            if (tri.a.w < depthBuffer[idx]) {
                Vert2D q(x - tri.a.x, y - tri.a.y);

                float s = q.cross(vs2) / crossVs;
                float t = vs1.cross(q) / crossVs;

                if ((s >= 0) && (t >= 0) && (s + t <= 1)) {
                    {
                        drawPixel(x, y, r, g, b);
                        depthBuffer[idx] = tri.a.w;
                    }
                }
            }
        }
    }
}

void Rasterizer::processMesh(Mesh& mesh) {
    //std::cout << "R: " << static_cast<void*>(screenBuffer) << std::endl;
    for( auto &face_i : mesh.faces){ //Fix this. Appears to fail passing in mesh due to access issue.
        if( face_i.size() == 3) {
            //TODO: Update the mesh to store triangles instead?
            //Probably not, because then I can't process faces with more than three verts.
            Triangle triangle;
            triangle.a = mesh.verts[face_i[0]];
            triangle.b = mesh.verts[face_i[1]];
            triangle.c = mesh.verts[face_i[2]];
            Triangle tri;
            //std::cout << "Process Triangle" << std::endl;
            tri = processTriangle(triangle, mesh.transform);
            //std::cout << "End Process Triangle" << std::endl;
            Vert lightDir(0,0,-1);
            double l = sqrt(lightDir.x*lightDir.x+lightDir.y*lightDir.y+lightDir.z*lightDir.z);
            lightDir.x /= l;
            lightDir.y /= l;
            lightDir.z /= l;

            double dp = tri.norm.x*lightDir.x + tri.norm.y*lightDir.y + tri.norm.z*lightDir.z;
            fillTriangle(tri,255*dp,255*dp,255*dp);
            if( mesh.drawWireframe ){
                if( !( isnan(tri.a.w)  || isnan(tri.b.w)  || isnan(tri.c.w)) ){
                    drawLine((int)tri.a.x, (int)tri.a.y, (int)tri.b.x, (int)tri.b.y);
                    drawLine((int)tri.b.x, (int)tri.b.y, (int)tri.c.x, (int)tri.c.y);
                    drawLine((int)tri.a.x, (int)tri.a.y, (int)tri.c.x, (int)tri.c.y);
                }
            }
        }
    }
}