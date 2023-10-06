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
    //Process meshes
    for( Mesh& mesh_i : meshes){
        rasterizer->processMesh(mesh_i);
    }
    
    
    //rasterizer->drawLine(0,0,200,200);
    mover += 0.001;
    for( int i = 0; i < meshes.size(); i++ ){
        meshes[i].setLocation(i*sin(mover+30*i),i*cos(mover+30*i),-20+10*sin(mover));
        meshes[i].setRotation(2*sin(mover), 3*cos(mover*2), mover);
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
    for( int i = 0; i < 1; i++ ){
        Mesh mesh;
        mesh.setLocation(0,0,0);
        mesh.setRotation(0,0,0);
        
        ReadObj("cube.obj", mesh);
        meshes.push_back(mesh);
    }
    
    
    
    mover = 0;
}