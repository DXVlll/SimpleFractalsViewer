#ifndef POINTV_CPP
#define POINTV_CPP
#include "PointV.hpp"

namespace dxvll_space_2d{
    PointV::PointV() : PointV(0.0, 0.0) {}
    PointV::PointV(double x, double y) : vec(3, 1.0){
        this->x() = x;
        this->y() = y;
    }
    PointV::PointV(const PointV &p) : vec(3, 1.0){
        this->getVec() = p.getVec();
        //this->y() = p.y();
    }

    double &PointV::x(){
        return this->vec.at(0);
    }
    double &PointV::y(){
        return this->vec.at(1);
    }
    vector<double> &PointV::getVec(){
        return this->vec;
    }
    void PointV::setVec(vector<double> newVec){
        int lim = vec.size() < newVec.size() ? vec.size() : newVec.size();
        for (int i = 0; i < lim; ++i){
            vec.at(i) = newVec.at(i);
        }
    }
    const double &PointV::x()const{
        return this->vec.at(0);
    }
    const double &PointV::y()const{
        return this->vec.at(1);
    }
    const vector<double> &PointV::getVec()const{
        return this->vec;
    }

    PointV PointV::operator=(const PointV & other){
        this->getVec() = other.getVec();
        return (*this);
    }
    PointV PointV::operator+(const PointV & other) const{
        return PointV(this->x() + other.x(), this->y() + other.y());
    }
    PointV PointV::operator-(const PointV & other) const{
        return PointV(this->x() - other.x(), this->y() - other.y());
    }
    PointV PointV::operator*(const double & other) const{
        return PointV(this->x()*other, this->y()*other);
    }
    bool PointV::operator==(const PointV & other) const{
        return (this->x() == other.x()) && (this->y() == other.y());
    }
    bool PointV::operator!=(const PointV & other) const{
        return (this->x() != other.x()) || (this->y() != other.y());
    }
    double PointV::getDistanceToZero() const{
        return sqrt(this->x()*this->x() + this->y()*this->y());
    }
    string PointV::toString()const{
        ostringstream oss;
        oss << "(" << this->x() << ", " << this->y() << ")";
        return oss.str();
    }
}
#endif // POINTV_CPP
