#include <vector>
struct vert{
    
    double x, y, z;

    vert(double x=0, double y=0, double z=0) : x(x), y(y), z(z) {}
    
    vert operator+(const vert& other) const{
        return vert(x+other.x, y+other.y, z+other.z);
    }

    vert operator*(double scalar) const {
        return vert(x*scalar, y*scalar, z*scalar);
    }

};

struct face{
    std::vector<vert> verts;  
};

struct mesh{
    std::vector<face> faces;
};

