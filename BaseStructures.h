#pragma once
#include <vector>
#include <math.h>

struct Vert2D{
    float x,y;
    
    Vert2D(float x=0, float y=0) : x(x), y(y){}

    float cross(const Vert2D& other){
        return x*other.y - y*other.x;
    }
};

//3D vertex.
struct Vert{
    
    float x, y, z, w;

    Vert(float x=0, float y=0, float z=0, float w=0) : x(x), y(y), z(z), w(w){}
    
    Vert operator+(const Vert& other) const{
        return Vert(x+other.x, y+other.y, z+other.z, w+other.w);
    }

    Vert operator-(const Vert& other) const{
        return Vert(x-other.x, y-other.y, z-other.z, w-other.w);
    }

    Vert operator*(double scalar) const {
        return Vert(x*scalar, y*scalar, z*scalar, w);
    }

    Vert operator*(int scalar) const {
        return Vert(x*scalar, y*scalar, z*scalar, w);
    }

    Vert operator*(float scalar) const {
        return Vert(x*scalar, y*scalar, z*scalar, w);
    }

    Vert operator/(double scalar) const {
        return Vert(x/scalar, y/scalar, z/scalar, w);
    }

    Vert operator/(float scalar) const {
        return Vert(x/scalar, y/scalar, z/scalar, w);
    }
    Vert cross(const Vert& other) const{
        return Vert(y*other.z-z*other.y, z*other.x-x*other.z, x*other.y-y*other.x, 0);
    }

    Vert normalize() const{
        float l = sqrt(x*x + y*y + z*z);
        if(l!=0){
            return Vert(x/l, y/l, z/l, w);
        } else{
            return Vert(x,y,z,w);
        }
    }

    float dot(const Vert& other) const{
        return x*other.x + y*other.y + z*other.z;
    }

    float length() const{
        return sqrt(x*x+y*y+z*z);
    }
};

struct Triangle{
    Vert a,b,c, norm;
};

struct Matrix4x4{
    float data[4][4] = {0};

    Vert operator*(const Vert& other) const{
        return Vert(other.x*data[0][0] + other.y*data[0][1] + other.z*data[0][2] + other.w*data[0][3] ,
                    other.x*data[1][0] + other.y*data[1][1] + other.z*data[1][2] + other.w*data[1][3] ,
                    other.x*data[2][0] + other.y*data[2][1] + other.z*data[2][2] + other.w*data[2][3] ,
                    other.x*data[3][0] + other.y*data[3][1] + other.z*data[3][2] + other.w*data[3][3]);
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;

        result.data[0][0] = data[0][0]*other.data[0][0] + data[0][1]*other.data[1][0] + data[0][2]*other.data[2][0] + data[0][3]*other.data[3][0];
        result.data[0][1] = data[0][0]*other.data[0][1] + data[0][1]*other.data[1][1] + data[0][2]*other.data[2][1] + data[0][3]*other.data[3][1];
        result.data[0][2] = data[0][0]*other.data[0][2] + data[0][1]*other.data[1][2] + data[0][2]*other.data[2][2] + data[0][3]*other.data[3][2];
        result.data[0][3] = data[0][0]*other.data[0][3] + data[0][1]*other.data[1][3] + data[0][2]*other.data[2][3] + data[0][3]*other.data[3][3];

        result.data[1][0] = data[1][0]*other.data[0][0] + data[1][1]*other.data[1][0] + data[1][2]*other.data[2][0] + data[1][3]*other.data[3][0];
        result.data[1][1] = data[1][0]*other.data[0][1] + data[1][1]*other.data[1][1] + data[1][2]*other.data[2][1] + data[1][3]*other.data[3][1];
        result.data[1][2] = data[1][0]*other.data[0][2] + data[1][1]*other.data[1][2] + data[1][2]*other.data[2][2] + data[1][3]*other.data[3][2];
        result.data[1][3] = data[1][0]*other.data[0][3] + data[1][1]*other.data[1][3] + data[1][2]*other.data[2][3] + data[1][3]*other.data[3][3];

        result.data[2][0] = data[2][0]*other.data[0][0] + data[2][1]*other.data[1][0] + data[2][2]*other.data[2][0] + data[2][3]*other.data[3][0];
        result.data[2][1] = data[2][0]*other.data[0][1] + data[2][1]*other.data[1][1] + data[2][2]*other.data[2][1] + data[2][3]*other.data[3][1];
        result.data[2][2] = data[2][0]*other.data[0][2] + data[2][1]*other.data[1][2] + data[2][2]*other.data[2][2] + data[2][3]*other.data[3][2];
        result.data[2][3] = data[2][0]*other.data[0][3] + data[2][1]*other.data[1][3] + data[2][2]*other.data[2][3] + data[2][3]*other.data[3][3];

        result.data[3][0] = data[3][0]*other.data[0][0] + data[3][1]*other.data[1][0] + data[3][2]*other.data[2][0] + data[3][3]*other.data[3][0];
        result.data[3][1] = data[3][0]*other.data[0][1] + data[3][1]*other.data[1][1] + data[3][2]*other.data[2][1] + data[3][3]*other.data[3][1];
        result.data[3][2] = data[3][0]*other.data[0][2] + data[3][1]*other.data[1][2] + data[3][2]*other.data[2][2] + data[3][3]*other.data[3][2];
        result.data[3][3] = data[3][0]*other.data[0][3] + data[3][1]*other.data[1][3] + data[3][2]*other.data[2][3] + data[3][3]*other.data[3][3];

        return result;
    }

    void identity(){
        data[0][0] = 1;
        data[1][1] = 1;
        data[2][2] = 1;
        data[3][3] = 1;
    }
};


struct Mesh{
    std::vector<Vert> verts;
    std::vector<std::vector<int>> faces; //Index of verts that are connected (Draw lines for each of these)
    Matrix4x4 transform;
    bool drawWireframe = false;

    void setLocation(float x, float y, float z){
        transform.data[0][3] = x;
        transform.data[1][3] = y;
        transform.data[2][3] = z;
        transform.data[3][3] = 1;
    }
    void setRotation(float pitch, float yaw, float roll){
        
        float cy = cos(yaw);
        float cr = cos(roll);
        float cp = cos(pitch);
        float sy = sin(yaw);
        float sr = sin(roll);
        float sp = sin(pitch);

        transform.data[0][0] = cy * cr;
        transform.data[0][1] = cr * sp * sy - cp * sr;
        transform.data[0][2] = cp * cr * sy + sp * sr;
        
        transform.data[1][0] = cy * sr;
        transform.data[1][1] = cp * cr + sp * sy * sr;
        transform.data[1][2] = cp * sy * sr - cr * sp;

        transform.data[2][0] = -sy;
        transform.data[2][1] = cy * sp;
        transform.data[2][2] = cp * cy;
        transform.data[3][3] = 1;
    }

    void optimize(){
        verts.shrink_to_fit();
        for(auto& face : faces){
            face.shrink_to_fit();
        }
        faces.shrink_to_fit();
    }
};

