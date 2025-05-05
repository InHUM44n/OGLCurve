#include <vector>
#include <cmath>

namespace curves
{
    enum CurveType
    {
        CubicBezier,
        Lagrange
    };
    std::vector<float> genCubicBezierCurve(int numPoints, const std::vector<float> &p0, const std::vector<float> &p1, const std::vector<float> &p2, const std::vector<float> &p3)
    {
        std::vector<float> points = p0;
        for (int i = 1; i <= numPoints; i++)
        {
            float t = (float)i / (float)numPoints;
            points.push_back(pow(1 - t, 3) * p0[0] + 3 * pow(1 - t, 2) * t * p1[0] + 3 * (1 - t) * pow(t, 2) * p2[0] + pow(t, 3) * p3[0]);
            points.push_back(pow(1 - t, 3) * p0[1] + 3 * pow(1 - t, 2) * t * p1[1] + 3 * (1 - t) * pow(t, 2) * p2[1] + pow(t, 3) * p3[1]);
        }
        return points;
    }
    std::vector<float> genCrosses(const std::vector<std::vector<float>*>& points)
    {
        std::vector<float> returnPoints;
            for (std::vector<float>* p : points)
        {
            // create coordinates for cross-stroke drawing
            for (int i = 0; i < points.size(); i++)
            {
                returnPoints.push_back((*p)[0] - 0.02);
                returnPoints.push_back((*p)[1] - 0.02);
                returnPoints.push_back((*p)[0] + 0.02);
                returnPoints.push_back((*p)[1] + 0.02);
                returnPoints.push_back((*p)[0] - 0.02);
                returnPoints.push_back((*p)[1] + 0.02);
                returnPoints.push_back((*p)[0] + 0.02);
                returnPoints.push_back((*p)[1] - 0.02);
            }
        }
        return returnPoints;
    }
}