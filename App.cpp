#include "App.h"
#include "ObjRead.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
App::App(int screen_width, int screen_height){
    //Rasterizer(Camera* cam, double fov, char* buffer, int screen_width, int screen_height){
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    
    cam.pos = Vert(0,0,-5,0);
    buffer = (char*)malloc(screen_height*screen_width*3);
    //std::cout << "A: " << static_cast<void*>(buffer) << std::endl; Debug print to verify address is the same.
    rasterizer = new Rasterizer(&cam, 80*3.14/180, buffer, screen_width, screen_height);
    init();
}

void App::update(){
    clearBuffer();
    
    //rasterizer->drawLine(0,0,200,200);
    
    //cam.pos = cam.pos + Vert( 0.01*isKeyDown('D') - 0.01*isKeyDown('A'),0,0.01*isKeyDown('W') - 0.01*isKeyDown('S'),0);
    cam.pos = cam.pos + cam.forward*((0.01*isKeyDown('W')) - (0.01*isKeyDown('S'))) + cam.right*((0.01*isKeyDown('A')) - (0.01*isKeyDown('D')));
    cam.yaw = cam.yaw - 0.001*isKeyDown('H') + 0.001*isKeyDown('J');
    mover += 0.001;
    //for( Mesh& mesh_i : meshes) {
    //meshes[0].setRotation(2*sin(mover), 3*cos(mover*2), mover);
    //}
    
    rasterizer->update();
    //Process meshes
    for( Mesh& mesh_i : meshes){
        rasterizer->processMesh(mesh_i);
    }
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

void App::init(){
   
    Mesh mesh;
    mesh.setLocation(0,0,0);
    mesh.setRotation(0,0,0);
    
    ReadObj("teapot.obj", mesh);
    
    Mesh plane;
    plane.setLocation(0,-2,0);
    plane.setRotation(0,0,0);
    ReadObj("plane.obj", plane);

    meshes.push_back(mesh);
    meshes.push_back(plane);
    
    mover = 0;
}