#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "BaseStructures.h"
#include "ObjRead.h"
#include "BaseStructures.h"
#include <vector>
#include <iostream>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

const wchar_t* g_szClassName = L"myWindowClass";
const int size_x = 1000;
const int size_y = 1000;
double rotate_test = 0;
unsigned char* frameBuffer;

void UpdateBuffer(unsigned char* buffer);
void RandomDraw(unsigned char* buffer, int size_x, int size_y);
void drawLine(unsigned char* buffer, int x1, int y1, int x2, int y2);

mesh teapot;
mesh cube;

//Pointers to all meshes that will be drawn.
std::vector<mesh*> meshes;

//Just leftovers to verify that I can write to the screen.
void RandomDraw(unsigned char* buffer, int x1, int y1, int x2, int y2){
    
    for( int x = x1; x != x2; x++){
        for( int y = y1; y != y2; y++){

            *(buffer + (y*size_x + x)*3 + 0) = rand()%255+1;
            *(buffer + (y*size_x + x)*3 + 1) = rand()%255+1;
            *(buffer + (y*size_x + x)*3 + 2) = rand()%255+1;

        }
    }
}

//Push the data to the screen.
void PushBuffer(HWND hwnd){
    
    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
    bmpInfo.bmiHeader.biWidth = size_x;
    bmpInfo.bmiHeader.biHeight = size_y;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;  // 8-bit per pixel (As in. 8 bits for R, 8 Bits for G and 8 bits for B. For a total of 24)
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    PAINTSTRUCT ps;
    HDC hdc = GetDC(hwnd);
    UpdateBuffer(frameBuffer);
    StretchDIBits(hdc, 0, 0, size_x, size_y, 0, 0, size_x, size_y, frameBuffer, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, hdc);
    
}

void DrawPixel( unsigned char* buffer, int x, int y){

    if( x >= size_x || y >= size_y || x < 0 || y < 0){
        return;
    }
    *(buffer + (y*size_x + x)*3 + 0) = 255;
    *(buffer + (y*size_x + x)*3 + 1) = 255;
    *(buffer + (y*size_x + x)*3 + 2) = 255;
}

void ClearBuffer( unsigned char* buffer, int size_x, int size_y){
    for( int i = 0; i < size_x*size_y*3; i++){
        *(buffer+i) = 0;
    }
}


//Bresenham's line algorithm
void drawLine(unsigned char* buffer, int x1, int y1, int x2, int y2) {
    // Check if the line is vertical
    if (x2 == x1) {
        int yStart = std::min(y1, y2);
        int yEnd = std::max(y1, y2);
        for (int y = yStart; y <= yEnd; ++y) {
            DrawPixel(buffer, x1, y);
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
        for (int x = std::max(x1,0); x <= std::min(x2,size_x-1); ++x) {
            int y = static_cast<int>(y1 + slope * (x - x1));
            DrawPixel(buffer, x, y);
        }
    } else {
        // Handle steep slopes (loop over y values)
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        for (int y = std::max(0, y1); y <= std::min(y2, size_y-1); ++y) {
            int x = static_cast<int>(x1 + (y - y1) / slope);
            DrawPixel(buffer, x, y);
        }
    }
}

/*
(h/w*(1/tan(theta/2) 0 0 0
0 1/tan(theta/2) 0 0
0 0 zfar/(zfar-znear) 1
0 0 (-zfar*znear)/(zfar-znear) 0
*/
void UpdateBuffer(unsigned char* buffer){
    ClearBuffer(buffer, size_x, size_y);

    double zfar = 10000;
    double znear = 0.1;
    double fov = 80 * (M_PI / 180.0);
    double aspectRatio = size_x/size_y;

    double x=0,y=0,z=0;
    double pitch=0,yaw=0,roll=0;

    
    //pitch = seconds;
    //yaw = 3.2*seconds;
    rotate_test = rotate_test + 0.001;
    pitch = rotate_test/8;
    yaw = rotate_test;
    roll = rotate_test*1.3;
    z = -10;
    //x = cos(rotate_test*2);
    //y = rotate_test;
    //teapot.setLocation(x,y,z);
    //teapot.setRotation(pitch,yaw,roll);

    cube.setLocation(x,y,z);
    cube.setRotation(pitch*3, yaw*1.5, roll*2);

    Matrix4x4 projectionMatrix;
    projectionMatrix.data[0][0] = 1.0 / (aspectRatio * std::tan(fov/2.0));
    projectionMatrix.data[1][1] = 1.0 / std::tan(fov/2.0);
    projectionMatrix.data[2][2] = -(zfar / (zfar - znear));
    projectionMatrix.data[2][3] = -((zfar * znear) / (zfar - znear));
    projectionMatrix.data[3][2] = -1.0;
    projectionMatrix.data[3][3] = 0.0;
    //Lets just create a face and try to draw it.
   
    vert camPos(0,0,120,0), camtarget(0,0,-1,0);
    //camPos = vert(0,rotate_test,0,0);
    //camtarget = vert(4*sin(rotate_test*4),4*cos(rotate_test*4),z,0);
    vert forward = (camPos-camtarget).normalize();
    vert right = vert(0,1,0,0).cross(forward);
    vert up    = forward.cross(right);

    /*
    | Right_x   Right_y   Right_z  -dot(Right, Eye)  |
| Up_x      Up_y      Up_z     -dot(Up, Eye)     |
| -Forward_x -Forward_y -Forward_z -dot(Forward, Eye) |
| 0        0        0        1           |*/
    //double dot = norm.x * t1.x + norm.y*t1.y + norm.z*t1.z;
    
    Matrix4x4 viewMatrix;
    viewMatrix.data[0][0] = right.x;
    viewMatrix.data[0][1] = right.y;
    viewMatrix.data[0][2] = right.z;
    viewMatrix.data[0][3] = -right.dot(camPos);

    viewMatrix.data[1][0] = up.x;
    viewMatrix.data[1][1] = up.y;
    viewMatrix.data[1][2] = up.z;
    viewMatrix.data[1][3] = -up.dot(camPos);

    viewMatrix.data[2][0] = -forward.x;
    viewMatrix.data[2][1] = -forward.y;
    viewMatrix.data[2][2] = -forward.z;
    viewMatrix.data[2][3] = -forward.dot(camPos);

    viewMatrix.data[3][0] = 0;
    viewMatrix.data[3][1] = 0;
    viewMatrix.data[3][2] = 0;
    viewMatrix.data[3][3] = 1;
    //Lets try a view matrix FPS camera:
    float camPitch = 0, sinPitch = 0, cosYaw = 0, sinyaw = 0;

    
    
    //Loop though each mesh and draw.
    for( auto &mesh_i : meshes){
        for( auto &face_i : (*mesh_i).faces){
            if( face_i.size() == 3)
            {
                vert t1, t2, t3;
                t1 = (mesh_i->transform*mesh_i->verts[face_i[0]]);
                t2 = (mesh_i->transform*mesh_i->verts[face_i[1]]);
                t3 = (mesh_i->transform*mesh_i->verts[face_i[2]]);
                vert v1, v2, v3;
                v1 = viewMatrix*t1;
                v2 = viewMatrix*t2;
                v3 = viewMatrix*t3;

                vert norm, l1, l2;
                l1 = v2-v1;
                l2 = v3-v1;
                norm.x = l1.y*l2.z - l1.z*l2.y;
                norm.y = l1.z*l2.x - l1.x*l2.z;
                norm.z = l1.x*l2.y - l1.y*l2.x;
                double l = sqrt(norm.x*norm.x + norm.y*norm.y + norm.z*norm.z);
                norm = norm/l;
                double dot = norm.x * (v1.x-camPos.x) + norm.y*(v1.y-camPos.y) + norm.z*(v1.z-camPos.z);
                if(dot < 0.0)
                {
                    vert s1 = projectionMatrix*v1;
                    s1 = s1/s1.w;
                    s1.x = ((s1.x+1.0)/2.0)*size_x;
                    s1.y = ((s1.y+1.0)/2.0)*size_y;
                    vert s2 = projectionMatrix*v2;
                    s2 = s2/s2.w;
                    s2.x = ((s2.x+1.0)/2.0)*size_x;
                    s2.y = ((s2.y+1.0)/2.0)*size_y;
                    vert s3 = projectionMatrix*v3;
                    s3 = s3/s3.w;
                    s3.x = ((s3.x+1.0)/2.0)*size_x;
                    s3.y = ((s3.y+1.0)/2.0)*size_y;
                    drawLine(buffer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y);
                    drawLine(buffer, (int)s2.x, (int)s2.y, (int)s3.x, (int)s3.y);
                    drawLine(buffer, (int)s3.x, (int)s3.y, (int)s1.x, (int)s1.y);
                }
            } /*else {
                for(int i = 0; i < face_i.size(); i++){
                    
                    
                    
                    vert transformed_vert_a = (*mesh_i).transform*(*mesh_i).verts[face_i[i]];

                    //calculate normal. Only need to do this once.
                    

                    vert screen_res_a = projectionMatrix*transformed_vert_a;
                
                    screen_res_a.x /= screen_res_a.w;
                    screen_res_a.y /= screen_res_a.w;
                    screen_res_a.z /= screen_res_a.w;
                    screen_res_a.x = ((screen_res_a.x+1.0)/2.0)*size_x;
                    screen_res_a.y = ((screen_res_a.y+1.0)/2.0)*size_y;
                    //DrawPixel(buffer, screen_res_a.x, screen_res_a.y);
                    
                    vert transformed_vert_b = (*mesh_i).transform*(*mesh_i).verts[face_i[(i+1)%face_i.size()]];
                    //After transformation
                    
                    
                    
                    vert screen_res_b = projectionMatrix*transformed_vert_b;

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
    }
}

void Init(){
    //ReadObj("teapot.obj", teapot);
    //meshes.push_back(&teapot);
    ReadObj("cube.obj", cube);
    meshes.push_back(&cube);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_PAINT:
        {
            auto start_time = std::chrono::high_resolution_clock::now();
            PushBuffer(hwnd);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            std::cout << duration <<" mS" << "\r" << std::flush;
        }
        break;
        case WM_CLOSE:
            free(frameBuffer);
            DestroyWindow(hwnd);
        break;
        
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXW wc;
    HWND hwnd;
    MSG Msg;

    frameBuffer = (unsigned char*)malloc(sizeof(unsigned char)*size_x*size_y*3);
    
    //Register window class.
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, size_x, size_y,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    Init();
    bool isRunning = true;
    while (isRunning){
        while(PeekMessage(&Msg, NULL, 0,0,PM_REMOVE))
        {
            if (Msg.message == WM_QUIT){
                isRunning = false;
            }
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        
    }
    return Msg.wParam;
}