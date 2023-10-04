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
public:
    App(int screen_width, int screen_height);
    void update();
    void init();
    void clearBuffer();
    char* getBuffer() {return buffer;}
};