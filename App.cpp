#include "App.h"
#include "ObjRead.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <math.h>
#define _USE_MATH_DEFINES

App::App(int screen_width, int screen_height){
    //Rasterizer(Camera* cam, double fov, char* buffer, int screen_width, int screen_height){
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    
    cam.pos = Vert(0,0,0,0);
    buffer = (char*)malloc(screen_height*screen_width*3);
    
    //std::cout << "A: " << static_cast<void*>(buffer) << std::endl; Debug print to verify address is the same.
    rasterizer = new Rasterizer(&cam, 70, buffer, screen_width, screen_height);
    init();
}

void App::init(){
   
    Mesh mesh;
    mesh.setLocation(0,0,10);
    mesh.setRotation(0,0,0);
    mesh.optimize();
    //mesh.drawWireframe = true;

    ReadObj("teapot.obj", mesh);
    
    Mesh plane;
    plane.setLocation(0,-1,0);
    plane.setRotation(0,0,0);
    ReadObj("plane.obj", plane);
    plane.drawWireframe = true;

    meshes.push_back(mesh);
    meshes.push_back(plane);
    
    mover = 0;

    mx = 0;
    my = 0;
    mdx = 0;
    mdy = 0;
    last_mx = -1;
    last_my = -1;
}

void App::update(){
    clearBuffer();
    
    //rasterizer->drawLine(0,0,200,200);
    
    //cam.pos = cam.pos + Vert( 0.01*isKeyDown('D') - 0.01*isKeyDown('A'),0,0.01*isKeyDown('W') - 0.01*isKeyDown('S'),0);
    cam.pos = cam.pos + (cam.forward*((0.01*isKeyDown('W')) - (0.01*isKeyDown('S')))) + (cam.right*(-(0.01*isKeyDown('A')) + (0.01*isKeyDown('D')))) + (cam.up*((0.01*isKeyDown(' ')) - (0.01*isKeyDown('C'))));
    //cam.yaw = cam.yaw - 0.001*isKeyDown('H') + 0.001*isKeyDown('J');
    
    cam.yaw = cam.yaw + 0.01*mdx;
    cam.pitch = cam.pitch + 0.01*mdy;
    if( cam.pitch > 89.0*M_PI/180){
        cam.pitch = 89.0*M_PI/180;
    } 
    if (cam.pitch < -89.0*M_PI/180 ){
        cam.pitch = -89.0*M_PI/180;
    }
    //std::cout << "MY: " << my << std::endl;
    //std::cout << "Pitch: " << cam.pitch * 180/M_PI << " Yaw: " << cam.yaw* 180/M_PI << std::endl;
    mover += 0.001;
    //for( Mesh& mesh_i : meshes) {
    //meshes[0].setRotation(2*sin(mover), 3*cos(mover*2), mover);
    //}
    
    rasterizer->update();
    //Process meshes
    for( Mesh& mesh_i : meshes){
        rasterizer->processMesh(mesh_i);
    }

    mdx = 0;
    mdy = 0;
}

void App::mouseSetPos(int x, int y){
    if(last_mx == -1 ){
        last_mx = x;
    }
    if(last_my == -1){
        last_my = y;
    }
    mx = x;
    my = y;
    
    mdx = x-last_mx;
    mdy = y-last_my;

    last_mx = x;
    last_my = y;
    //std::cout << "mdx: " << mdx << " mdy: " << mdy << std::endl;
}

bool App::isKeyDown(char key){
    return std::find(keysDown.begin(), keysDown.end(), key) != keysDown.end();
}

void App::keyDown(char key){
    keysDown.push_back(key);
}

void App::keyUp(char key){
    keysDown.erase(std::remove(keysDown.begin(), keysDown.end(), key), keysDown.end());
}

void App::clearBuffer(){
    memset(buffer, 0, screen_height*screen_width*3);
}

