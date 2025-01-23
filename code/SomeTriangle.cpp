
#ifndef SOME_TRIANGLE_CPP
#define SOME_TRIANGLE_CPP

#include "SomeTriangle.hpp"

namespace dxvll_space_2d{
    SomeTriangle::SomeTriangle (): p1(), p2(), p3(){

    }

    SomeTriangle::SomeTriangle (PointV p1, PointV p2, PointV p3){
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
    }
    SomeTriangle::SomeTriangle (const SomeTriangle &other):SomeTriangle(other.p1, other.p2, other.p3){}

    PointV SomeTriangle::getPoint(int id)const{
        switch(id){
            case 1:
            return p1;
            case 2:
            return p2;
            case 3:
            return p3;
            default:
            throw "Triangle has only 3 points: '1', '2' and '3'";
        }
    }

    void SomeTriangle::setPoint(int id, PointV value){
        switch(id){
            case 1:
            p1 = value;
            return;
            case 2:
            p2 = value;
            return;
            case 3:
            p3 = value;
            return;
            default:
            throw "Triangle has only 3 points: '1', '2' and '3'";
        }
    }
    void SomeTriangle::shift(PointV bias){
        p1 = p1 + bias;
        p2 = p2 + bias;
        p3 = p3 + bias;
    }

    string SomeTriangle::toString()const{
        ostringstream oss;
        oss << "{[T]" << p1.toString() << ", " << p2.toString() << ", " << p3.toString() << "}";
        return oss.str();
    }
}

#endif // SOME_TRIANGLE_CPP
