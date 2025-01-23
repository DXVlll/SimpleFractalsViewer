
#ifndef SOME_CAMERA_CPP
#define SOME_CAMERA_CPP

#include "SomeCamera.hpp"
namespace dxvll_space_2d{
    SomeCamera::SomeCamera() : SomeCamera(0.0, 0.0, 1.0, 50, 50) {}
    SomeCamera::SomeCamera(double x, double y, double scale, int cameraWidth, int cameraHeight) : position(x, y), 
                scale(scale), cameraHeight(cameraHeight), cameraWidth(cameraWidth) {}

    int SomeCamera::convertX(double x){
        return (int)((x - this->position.x())*this->scale) + cameraWidth/2;
    }
    int SomeCamera::convertY(double y){
        return (int)((y - this->position.y())*this->scale) + cameraHeight/2;
    }
    double SomeCamera::uncoverX(int x){
        return (double)(x-cameraWidth/2)/this->scale + this->position.x();
    }
    double SomeCamera::uncoverY(int y){
        return (double)(y-cameraHeight/2)/this->scale + this->position.y();
    }
    void SomeCamera::zoom(double coefficient){
        this->scale *= coefficient;
    }
    void SomeCamera::move(PointV bias){
        this->position = this->position + bias;
    }

    double SomeCamera::getX(){
        return this->position.x();
    }
    void SomeCamera::setX(double x){
        this->position.x() = x;
    }
    double SomeCamera::getY(){
        return this->position.y();
    }
    void SomeCamera::setY(double y){
        this->position.y() = y;
    }
    double SomeCamera::getScale(){
        return this->scale;
    }
    void SomeCamera::setScale(double scale){
        this->scale = scale;
    }
}

#endif // SOME_CAMERA_CPP
