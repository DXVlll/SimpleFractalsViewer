
#ifndef SOME_TRIANGLE_HPP
#define SOME_TRIANGLE_HPP

#include "PointV.hpp"

namespace dxvll_space_2d{
    class SomeTriangle {
        private:
        PointV p1;
        PointV p2;
        PointV p3;

        public:
        SomeTriangle ();
        SomeTriangle (PointV p1, PointV p2, PointV p3);
        SomeTriangle (const SomeTriangle &other);

        PointV getPoint(int id) const; // get value of triangle point (acceptable values: 1, 2, 3)
        void setPoint(int id, PointV value); // set value of triangle point (acceptable values: 1, 2, 3)
        void shift(PointV bias); // move all triangle points by bias

        string toString()const;
    };
}


#endif // SOME_TRIANGLE_HPP
