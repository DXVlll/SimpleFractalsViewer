#ifndef SOME_LINE_CPP
#define SOME_LINE_CPP
#include "SomeLine.hpp"
namespace dxvll_space_2d{
    SomeLine::SomeLine() : p1(), p2() {}
    SomeLine::SomeLine(PointV p1, PointV p2) : p1(p1), p2(p2) {}
}
#endif // SOME_LINE_CPP
