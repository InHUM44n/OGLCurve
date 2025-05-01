#include "curves.cpp"

namespace curves
{
    std::vector<float> genCubicBezierCurve(int numPoints, const std::vector<float> &p0, const std::vector<float> &p1, const std::vector<float> &p2, const std::vector<float> &p3);
    std::vector<float> genCrosses(const std::vector<std::vector<float> *> &points);
}