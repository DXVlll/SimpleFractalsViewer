
#ifndef POINTV_HPP
#define POINTV_HPP
#include <cmath>
#include<string>
#include<sstream>

#include <vector>

namespace dxvll_space_2d{
    using namespace std;

    class PointV{
    private:
        vector<double> vec;
    public:

        
        PointV();
        PointV(double x, double y);
        PointV(const PointV &p);

        double &x();
        double &y();
        vector<double> &getVec();
        void setVec(vector<double> newVec);
        const double &x()const;
        const double &y()const;
        const vector<double> &getVec() const;

        PointV operator=(const PointV & other);
        PointV operator+(const PointV & other) const; // We can add points together by adding their x and y coordinates by using + operator
        PointV operator-(const PointV & other) const;
        PointV operator*(const double & other) const;
        bool operator==(const PointV & other) const; // We can compare points by comparing their x and y coordinates by using == operator
        bool operator!=(const PointV & other) const;
        double getDistanceToZero() const; // Calculate distance from point (0,0) to (*this) point
        string toString()const;
    };
}


#endif // POINTV_HPP
