#pragma once

#include <vector>
#include <cmath>

namespace curves
{
    enum class CurveType
    {
        CubicBezier,
        Lagrange
    };
    std::vector<float> genCubicBezierCurve(int numPoints, const std::vector<float>& p0, const std::vector<float>& p1, const std::vector<float>& p2, const std::vector<float>& p3);
    std::vector<float> genLagrangeCurve(int numPoints, const std::vector<std::vector<float>>& points);
    std::vector<float> genCrosses(const std::vector<std::vector<float>>& points);
}