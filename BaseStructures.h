#pragma once
#include <vector>
#include <math.h>

struct vert2D{

    double x,y;
    vert2D(double x=0, double y=0) : x(x), y(y) {};

    vert2D operator+(const vert2D& other) const{
        return vert2D(x+other.x, y+other.y);
    }

    vert2D operator*(double scalar) const {
        return vert2D(x*scalar, y*scalar);
    }

    

};


//3D vertex.
struct vert{
    
    double x, y, z, w;

    vert(double x=0, double y=0, double z=0, double w=0) : x(x), y(y), z(z), w(w){}
    
    vert operator+(const vert& other) const{
        return vert(x+other.x, y+other.y, z+other.z, w+other.w);
    }

    vert operator*(double scalar) const {
        return vert(x*scalar, y*scalar, z*scalar, w*scalar);
    }

};





struct Matrix4x4{
    double data[4][4] = {0};

    vert operator*(const vert& other) const{
        return vert(other.x*data[0][0] + other.y*data[0][1] + other.z*data[0][2] + other.w*data[0][3] ,
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


struct mesh{
    std::vector<vert> verts;
    std::vector<int> faces; //Index of verts that are connected (Draw lines for each of these)
    Matrix4x4 transform;

    void setLocation(int x, int y, int z){
        transform.data[0][3] = x;
        transform.data[1][3] = y;
        transform.data[2][3] = z;
    }
    void setRotation(int pitch, int yaw, int roll){
        
        double cy = cos(yaw);
        double cr = cos(roll);
        double cp = cos(pitch);
        double sy = sin(yaw);
        double sr = sin(roll);
        double sp = sin(pitch);

        transform.data[0][0] = cy * cr;
        transform.data[0][1] = cr * sp * sy - cp * sr;
        transform.data[0][2] = cp * cr * sy + sp * sr;
        
        transform.data[1][0] = cy * sr;
        transform.data[1][1] = cp * cr + sp * sy * sr;
        transform.data[1][2] = cp * sy * sr - cr * sp;

        transform.data[2][0] = -sy;
        transform.data[2][1] = cy * sp;
        transform.data[2][2] = cp * cy;
    }
};