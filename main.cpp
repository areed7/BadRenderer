#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "BaseStructures.h"

const wchar_t* g_szClassName = L"myWindowClass";
const int size = 800;
unsigned char* frameBuffer;
// Step 4: the Window Procedure

void UpdateBuffer(unsigned char* buffer);
void RandomDraw(unsigned char* buffer, int size_x, int size_y);
void DrawCircle(unsigned char* buffer, int size_x, int size_y, int centerX, int centerY, int R);

void RandomDraw(unsigned char* buffer, int size_x, int size_y){
    for( int i = 0; i < size_x*size_y*3; i++){
        *(buffer+i) = rand()%255+1;
    }
}

void PushBuffer(HWND hwnd){
    
    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
    bmpInfo.bmiHeader.biWidth = size;
    bmpInfo.bmiHeader.biHeight = size;  // Top-down
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;  // 8-bit per pixel (As in. 8 bits for R, 8 Bits for G and 8 bits for B. For a total of 24)
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    PAINTSTRUCT ps;
    HDC hdc = GetDC(hwnd);
    UpdateBuffer(frameBuffer);
    StretchDIBits(hdc, 0, 0, size, size, 0, 0, size, size, frameBuffer, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, hdc);
    
    
}

void ClearBuffer( unsigned char* buffer, int size_x, int size_y){
    for( int i = 0; i < size_x*size_y*3; i++){
        *(buffer+i) = 0;
    }
}

void DrawCircle(unsigned char* buffer, int size_x, int size_y, int centerX, int centerY, int R) {
    for (int y = -R; y <= R; y++) {
        for (int x = -R; x <= R; x++) {
            if (x * x + y * y <= R * R) {
                int pixelX = centerX + x;
                int pixelY = centerY + y;

                // Ensure we're within the buffer bounds
                if (pixelX >= 0 && pixelX < size_x && pixelY >= 0 && pixelY < size_y) {
                    int index = (pixelY * size_x + pixelX) * 3; // *3 for RGB

                    buffer[index] = 255;     // R
                    buffer[index + 1] = 255; // G
                    buffer[index + 2] = 255; // B
                }
            }
        }
    }
}

void UpdateBuffer(unsigned char* buffer){
    ClearBuffer(buffer, size, size);
    time_t timer;
    time(&timer);
    DrawCircle(buffer, size,size,timer%size, size/2, 20);
    //RandomDraw(frameBuffer, size, size);
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

    frameBuffer = (unsigned char*)malloc(sizeof(unsigned char)*size*size*3);
    
    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = (CS_VREDRAW|CS_HREDRAW);
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

    // Step 2: Creating the Window
    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, size, size,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Step 3: The Message Loop
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