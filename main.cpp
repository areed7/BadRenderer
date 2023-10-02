#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
//#include "BaseStructures.h"
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
// Step 4: the Window Procedure

void UpdateBuffer(unsigned char* buffer);
void RandomDraw(unsigned char* buffer, int size_x, int size_y);
void drawLine(unsigned char* buffer, int x1, int y1, int x2, int y2);


 std::vector<vert> verts;

void RandomDraw(unsigned char* buffer, int x1, int y1, int x2, int y2){
    
    for( int x = x1; x != x2; x++){
        for( int y = y1; y != y2; y++){

            *(buffer + (y*size_x + x)*3 + 0) = rand()%255+1;
            *(buffer + (y*size_x + x)*3 + 1) = rand()%255+1;
            *(buffer + (y*size_x + x)*3 + 2) = rand()%255+1;

        }
    }
}

void PushBuffer(HWND hwnd){
    
    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
    bmpInfo.bmiHeader.biWidth = size_x;
    bmpInfo.bmiHeader.biHeight = size_y;  // Top-down
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
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int error = dx + dy;
    
    while(1){
        DrawPixel(buffer, x1, y1);
        if( x1 == x2 && y1 == y2 ) break;
        int e2 = 2 * error;
        if (e2 >= dy){
            if (x1 == x2) break;
            error = error + dy;
            x1 = x1 + sx;
        }
        if(e2 <= dx){
            if(y1 == y2) break;
            error = error + dx;
            y1 = y1 + sy;
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
    //DrawPixel(buffer, 800, 250);
    //DrawPixel(buffer, 250, 250);
    //drawLine(buffer, 0,0,size_x-1, size_y-1);

    double zfar = 10000;
    double znear = 0.1;
    double fov = 80 * (M_PI / 180.0);
    double aspectRatio = size_x/size_y;

    double x=0,y=0,z=0;
    double pitch=0,yaw=0,roll=0;

    
    //pitch = seconds;
    //yaw = 3.2*seconds;
    rotate_test = rotate_test + 0.001;
    pitch = 2*M_PI*sin(rotate_test);
    yaw = rotate_test;
    //roll = rotate_test*1.3;
    z = -30 + 60*cos(rotate_test);
    x = cos(rotate_test*100);
    y = sin(rotate_test*100);
    Matrix4x4 boxTransform;
    boxTransform.data[0][0] = cos(yaw) * cos(roll);
    boxTransform.data[0][1] = cos(roll) * sin(pitch) * sin(yaw) - cos(pitch) * sin(roll);
    boxTransform.data[0][2] = cos(pitch) * cos(roll) * sin(yaw) + sin(pitch) * sin(roll);
    boxTransform.data[0][3] = x;

    boxTransform.data[1][0] = cos(yaw) * sin(roll);
    boxTransform.data[1][1] = cos(pitch) * cos(roll) + sin(pitch) * sin(yaw) * sin(roll);
    boxTransform.data[1][2] = cos(pitch) * sin(yaw) * sin(roll) - cos(roll) * sin(pitch);
    boxTransform.data[1][3] = y;

    boxTransform.data[2][0] = -sin(yaw);
    boxTransform.data[2][1] = cos(yaw) * sin(pitch);
    boxTransform.data[2][2] = cos(pitch) * cos(yaw);
    boxTransform.data[2][3] = z;
    boxTransform.data[3][3] = 1;

    


    Matrix4x4 projectionMatrix;
    projectionMatrix.data[0][0] = 1.0 / (aspectRatio * std::tan(fov/2.0));
    projectionMatrix.data[1][1] = 1.0 / std::tan(fov/2.0);
    projectionMatrix.data[2][2] = -(zfar / (zfar - znear));
    projectionMatrix.data[2][3] = -((zfar * znear) / (zfar - znear));
    projectionMatrix.data[3][2] = -1.0;
    projectionMatrix.data[3][3] = 0.0;
    //Lets just create a face and try to draw it.
   
    /*verts.push_back(vert(1,1,-1,1));
    verts.push_back(vert(-1,1,-1,1));
    verts.push_back(vert(1,-1,-1,1));
    verts.push_back(vert(-1,-1,-1,1));
    verts.push_back(vert(1,1,1,1));
    verts.push_back(vert(-1,1,1,1));
    verts.push_back(vert(1,-1,1,1));
    verts.push_back(vert(-1,-1,1,1));
    */
    



    
    vert last_pixel;
    

    std::vector<vert> screen_verts;
    for( auto &vert_i : verts){
        vert transformed_vert = boxTransform*vert_i;
        vert screen_res = projectionMatrix*transformed_vert;
        //std::cout << "X: " << screen_res.x << " Y: " << screen_res.y << std::endl;
        
        screen_res.x /= screen_res.w;
        screen_res.y /= screen_res.w;
        screen_res.z /= screen_res.w;

        screen_res.x = ((screen_res.x+1.0)/2.0)*size_x;
        screen_res.y = ((screen_res.y+1.0)/2.0)*size_y;
        DrawPixel(buffer, screen_res.x, screen_res.y);
        //drawLine(buffer, (int)screen_res.x, (int)screen_res.y, (int)last_pixel.x, (int)last_pixel.y);
        //screen_verts.push_back(screen_res);
    }

    /*for( int i = 0 ; i < screen_verts.size(); i++ ){
        for( int j = 0; j < screen_verts.size(); j++){
            drawLine(buffer, (int)screen_verts[i].x, (int)screen_verts[i].y, (int)screen_verts[j].x, (int)screen_verts[j].y);
        }
    }*/

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_PAINT:
        {
            PushBuffer(hwnd);
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
    ReadObj("teapot.obj", verts);
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