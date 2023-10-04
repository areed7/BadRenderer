#include "App.h"
#include "ObjRead.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
App::App(int screen_width, int screen_height){
    //Rasterizer(Camera* cam, double fov, char* buffer, int screen_width, int screen_height){
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    
    cam.pos = Vert(0,0,0,0);
    cam.target = Vert(0,0,-1,0);
    buffer = (char*)malloc(screen_height*screen_width*3);
    //std::cout << "A: " << static_cast<void*>(buffer) << std::endl; Debug print to verify address is the same.
    rasterizer = new Rasterizer(&cam, 80*3.14/180, buffer, screen_width, screen_height);
    init();
}

void App::update(){
    clearBuffer();
    rasterizer->update();
    rasterizer->processMesh(meshes[0]);
    //rasterizer->drawLine(0,0,200,200);
    mover += 0.001;
    meshes[0].setLocation(2*sin(mover),2*cos(mover),-20+10*sin(mover));
    meshes[0].setRotation(2*sin(mover), 3*cos(mover*2), mover);
}

void App::clearBuffer(){
    memset(buffer, 0, screen_height*screen_width*3);
}

void App::init(){
    Mesh mesh;
    mesh.setLocation(0,0,0);
    mesh.setRotation(0,0,0);
    ReadObj("teapot.obj", mesh);
    meshes.push_back(mesh);
    mover = 0;
}