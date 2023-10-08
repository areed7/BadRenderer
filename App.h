#include "Rasterizer.h"
#include <vector>
class App{
    int screen_width;
    int screen_height;
    char* buffer;
    Camera cam;
    Rasterizer* rasterizer;
    double mover;
    std::vector<Mesh> meshes;
    std::vector<char> keysDown;

    int last_mx, last_my;
    int mdx, mdy;
    int mx, my;

public:
    App(int screen_width, int screen_height);
    void update();
    void init();
    void clearBuffer();
    char* getBuffer() {return buffer;}
    void keyDown(char key);
    void keyUp(char key);
    bool isKeyDown(char key);
    void mouseSetPos(int x, int y);
};