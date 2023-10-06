#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <chrono>
#include "App.h"

#define _USE_MATH_DEFINES
#include <math.h>

const wchar_t* g_szClassName = L"myWindowClass";
const int screen_width = 1000;
const int screen_height = 1000;
double rotate_test = 0;
unsigned char* frameBuffer;

App* app;
BITMAPINFO bmpInfo;

//Push the data to the screen.
void PushBuffer(HWND hwnd){
    
    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
    bmpInfo.bmiHeader.biWidth = screen_width;
    bmpInfo.bmiHeader.biHeight = screen_height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;  // 8-bit per pixel (As in. 8 bits for R, 8 Bits for G and 8 bits for B. For a total of 24)
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    PAINTSTRUCT ps;
    HDC hdc = GetDC(hwnd);
    app->update();
    StretchDIBits(hdc, 0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, app->getBuffer(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, hdc);
    
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
            std::cout << duration <<"   mS" << "\r" << std::flush;
        }
        break;

        case WM_KEYDOWN:
            app->keyDown(wParam);
        break;

        case WM_KEYUP:
            app->keyUp(wParam);
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
    //Initalize the app.
    app = new App(screen_width, screen_height);
    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, screen_width, screen_height,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    
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