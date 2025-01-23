
#ifndef SOME_LINE_HPP
#define SOME_LINE_HPP

#include "PointV.hpp"
namespace dxvll_space_2d{
    class SomeLine {
        public:

        PointV p1;
        PointV p2;
        SomeLine ();
        SomeLine (PointV p1, PointV p2);

    };
}
#endif // SOME_LINE_HPP
