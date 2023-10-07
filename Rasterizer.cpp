#include "Rasterizer.h"
#include <math.h>
#include <cmath>
#include <iostream>

/* 
 * fov : FOV in radians
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

    //Only needs to be processed once.
    projectionMatrix.data[0][0] = 1.0 / (screen_width/screen_height * std::tan(fov/2.0));
    projectionMatrix.data[1][1] = 1.0 / std::tan(fov/2.0);
    projectionMatrix.data[2][2] = -(zfar / (zfar - znear));
    projectionMatrix.data[2][3] = -((zfar * znear) / (zfar - znear));
    projectionMatrix.data[3][2] = -1.0;
    projectionMatrix.data[3][3] = 0.0;

    //Needs to be processed for each frame.
    double cosPitch = cos(cam->pitch);
    double sinPitch = sin(cam->pitch);
    double cosYaw = cos(cam->yaw);
    double sinYaw = sin(cam->yaw);

    Vert forward(sinYaw * cosPitch, sinPitch, cosYaw * cosPitch,0);
    
    Vert a = Vert(0,1,0,0)*(Vert(0,1,0,0).dot(forward));
    Vert up= Vert(0,1,0,0) - a;
    up = up.normalize();
    
    Vert right      = up.cross(forward);

    viewMatrix.data[0][0] = right.x;
    viewMatrix.data[0][1] = right.y;
    viewMatrix.data[0][2] = right.z;
    viewMatrix.data[0][3] = -right.dot(cam->pos);

    viewMatrix.data[1][0] = up.x;
    viewMatrix.data[1][1] = up.y;
    viewMatrix.data[1][2] = up.z;
    viewMatrix.data[1][3] = -up.dot(cam->pos);

    viewMatrix.data[2][0] = -forward.x;
    viewMatrix.data[2][1] = -forward.y;
    viewMatrix.data[2][2] = -forward.z;
    viewMatrix.data[2][3] = -forward.dot(cam->pos);

    viewMatrix.data[3][0] = 0;
    viewMatrix.data[3][1] = 0;
    viewMatrix.data[3][2] = 0;
    viewMatrix.data[3][3] = 1;

}

void Rasterizer::update(){

    double cosPitch = cos(cam->pitch);
    double sinPitch = sin(cam->pitch);
    double cosYaw = cos(cam->yaw);
    double sinYaw = sin(cam->yaw);

    cam->forward = Vert(sinYaw * cosPitch, sinPitch, cosYaw * cosPitch,0);
    
    Vert a = Vert(0,1,0,0)*(Vert(0,1,0,0).dot(cam->forward));
    cam->up= Vert(0,1,0,0) - a;
    cam->up = cam->up.normalize();
    
    cam->right      = cam->up.cross(cam->forward);

    viewMatrix.data[0][0] = cam->right.x;
    viewMatrix.data[0][1] = cam->right.y;
    viewMatrix.data[0][2] = cam->right.z;
    viewMatrix.data[0][3] = -cam->right.dot(cam->pos);

    viewMatrix.data[1][0] = cam->up.x;
    viewMatrix.data[1][1] = cam->up.y;
    viewMatrix.data[1][2] = cam->up.z;
    viewMatrix.data[1][3] = -cam->up.dot(cam->pos);

    viewMatrix.data[2][0] = -cam->forward.x;
    viewMatrix.data[2][1] = -cam->forward.y;
    viewMatrix.data[2][2] = -cam->forward.z;
    viewMatrix.data[2][3] = -cam->forward.dot(cam->pos);

    viewMatrix.data[3][0] = 0;
    viewMatrix.data[3][1] = 0;
    viewMatrix.data[3][2] = 0;
    viewMatrix.data[3][3] = 1;
}

void Rasterizer::drawPixel(int x, int y){

    if( x >= screen_width || y >= screen_height || x < 0 || y < 0){
        return;
    }
    *(screenBuffer + (y*screen_width + x)*3 + 0) = 255;
    *(screenBuffer + (y*screen_width + x)*3 + 1) = 255;
    *(screenBuffer + (y*screen_width + x)*3 + 2) = 255;
}

//Bresenham's line algorithm
void Rasterizer::drawLine(int x1, int y1, int x2, int y2) {
    // Check if the line is vertical
    if (x2 == x1) {
        int yStart = std::min(y1, y2);
        int yEnd = std::max(y1, y2);
        for (int y = yStart; y <= yEnd; ++y) {
            drawPixel(x1, y);
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
            drawPixel(x, y);
        }
    } else {
        // Handle steep slopes (loop over y values)
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        for (int y = std::max(0, y1); y <= std::min(y2, screen_height-1); ++y) {
            int x = static_cast<int>(x1 + (y - y1) / slope);
            drawPixel(x, y);
        }
    }
}

/*
 * Takes a triangle in the world, and returns the corresponding triangle on the screen.
*/
Triangle Rasterizer::processTriangle(Triangle& vertex, Matrix4x4& mesh_transform){
    Vert t1, t2, t3;
    t1 = mesh_transform*vertex.a;
    t2 = mesh_transform*vertex.b;
    t3 = mesh_transform*vertex.c;

    Vert v1, v2, v3;
    v1 = viewMatrix*t1;
    v2 = viewMatrix*t2;
    v3 = viewMatrix*t3;

    Vert norm, l1, l2;
    l1 = v2-v1;
    l2 = v3-v1;
    norm = l1.cross(l2);
    norm = norm.normalize();

    double dot = norm.dot(cam->forward);

    if(dot > 0.0)
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
        return screenTriangle;
    }
    /*else { Bad old code that processes other types of faces. Ignore for now. 
    for(int i = 0; i < face_i.size(); i++){
        
        
        
        Vert transformed_vert_a = (*mesh_i).transform*(*mesh_i).verts[face_i[i]];

        //calculate normal. Only need to do this once.
        

        Vert screen_res_a = projectionMatrix*transformed_vert_a;
    
        screen_res_a.x /= screen_res_a.w;
        screen_res_a.y /= screen_res_a.w;
        screen_res_a.z /= screen_res_a.w;
        screen_res_a.x = ((screen_res_a.x+1.0)/2.0)*size_x;
        screen_res_a.y = ((screen_res_a.y+1.0)/2.0)*size_y;
        //DrawPixel(buffer, screen_res_a.x, screen_res_a.y);
        
        Vert transformed_vert_b = (*mesh_i).transform*(*mesh_i).verts[face_i[(i+1)%face_i.size()]];
        //After transformation
        
        
        
        Vert screen_res_b = projectionMatrix*transformed_vert_b;

        screen_res_b.x /= screen_res_b.w;
        screen_res_b.y /= screen_res_b.w;
        screen_res_b.z /= screen_res_b.w;
        screen_res_b.x = ((screen_res_b.x+1.0)/2.0)*size_x;
        screen_res_b.y = ((screen_res_b.y+1.0)/2.0)*size_y;
        if( screen_res_b.w > 0 || screen_res_a.w > 0) {
            drawLine(buffer, (int)screen_res_a.x, (int)screen_res_a.y, (int)screen_res_b.x, (int)screen_res_b.y);
        }
    }
}*/
}

void PrintTri(Triangle tri){
    std::cout << "A: " << tri.a.x << ", " << tri.a.y << ", " << tri.a.z << ", " << tri.a.w << std::endl;
    std::cout << "B: " << tri.b.x << ", " << tri.b.y << ", " << tri.b.z << ", " << tri.b.w << std::endl;
    std::cout << "C: " << tri.c.x << ", " << tri.c.y << ", " << tri.c.z << ", " << tri.c.w << std::endl;
}

void Rasterizer::fillTriangle(Triangle tri, int r, int g, int b){
    
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
            tri = processTriangle(triangle, mesh.transform);

            
            if( !( isnan(tri.a.w)  || isnan(tri.b.w)  || isnan(tri.c.w)) ){
                drawLine((int)tri.a.x, (int)tri.a.y, (int)tri.b.x, (int)tri.b.y);
                drawLine((int)tri.b.x, (int)tri.b.y, (int)tri.c.x, (int)tri.c.y);
                drawLine((int)tri.a.x, (int)tri.a.y, (int)tri.c.x, (int)tri.c.y);
            }
        }
    }
}