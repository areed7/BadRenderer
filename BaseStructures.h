#pragma once
#include <vector>
#include <math.h>

//3D vertex.
struct Vert{
    
    double x, y, z, w;

    Vert(double x=0, double y=0, double z=0, double w=0) : x(x), y(y), z(z), w(w){}
    
    Vert operator+(const Vert& other) const{
        return Vert(x+other.x, y+other.y, z+other.z, w+other.w);
    }

    Vert operator-(const Vert& other) const{
        return Vert(x-other.x, y-other.y, z-other.z, w-other.w);
    }

    Vert operator*(double scalar) const {
        return Vert(x*scalar, y*scalar, z*scalar, w*scalar);
    }

    Vert operator/(double scalar) const {
        return Vert(x/scalar, y/scalar, z/scalar, w/scalar);
    }
    Vert cross(const Vert& other) const{
        return Vert(y*other.z-z*other.y, z*other.x-x*other.z, x*other.y-y*other.x, 0);
    }

    Vert normalize() const{
        double l = sqrt(x*x + y*y + z*z + w*w);
        if(l!=0){
            return Vert(x/l, y/l, z/l, w/l);
        } else{
            return Vert(x,y,z,w);
        }
    }

    double dot(const Vert& other) const{
        return x*other.x + y*other.y + z*other.z +w*other.w;
    }
};

struct Triangle{
    Vert a,b,c;
};

struct Matrix4x4{
    double data[4][4] = {0};

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

    void setLocation(double x, double y, double z){
        transform.data[0][3] = x;
        transform.data[1][3] = y;
        transform.data[2][3] = z;
        transform.data[3][3] = 1;
    }
    void setRotation(double pitch, double yaw, double roll){
        
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
        transform.data[3][3] = 1;
    }
};

