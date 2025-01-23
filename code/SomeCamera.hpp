
#ifndef SOME_CAMERA_HPP
#define SOME_CAMERA_HPP

#include "SomeTransform.hpp"

namespace dxvll_space_2d{
    class SomeCamera {
    private:
        PointV position;
        SomeTransform transform;
        
        double scale;
        int cameraHeight;
        int cameraWidth;

    public:
        SomeCamera();
        SomeCamera(double x, double y, double scale, int cameraWidth, int cameraHeight);

        int convertX(double x); // convert geometry coordinate (points, lines, etc.. double) to screen coordinate.
        int convertY(double y);
        double uncoverX(int x); // convert screen  coordinate to geometry coordinate.
        double uncoverY(int y);
        void zoom(double coefficient);
        void move(PointV bias);

        double getX();
        void setX(double x);
        double getY();
        void setY(double y);
        double getScale();
        void setScale(double scale);
    };
}
#endif // SOME_CAMERA_HPP
