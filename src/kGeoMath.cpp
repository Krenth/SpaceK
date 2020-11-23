#include <tuple>
#include <cmath>
#include <boost/geometry.hpp>
#include "kGeoMath.hpp"

#define PI 3.14159265

namespace bg = boost::geometry;

std::tuple<double, double, double> attitudeToDirection(float pitch, float heading)
{
    bg::model::point<long double, 2, bg::cs::spherical<bg::degree>> dir_spherical(90 - heading, 90 - pitch);
    bg::model::point<long double, 3, bg::cs::cartesian> dir_cartesian;
    bg::transform(dir_spherical, dir_cartesian);
    std::tuple<double, double, double> directionVector;
    std::get<0>(directionVector) = bg::get<2>(dir_cartesian);
    std::get<1>(directionVector) = bg::get<1>(dir_cartesian);
    std::get<2>(directionVector) = bg::get<0>(dir_cartesian);
    return directionVector;
}

std::tuple<float, float> directionToAttitude(std::tuple<double, double, double> directionVector)
{
    double x, y, z;
    float pitch, heading;

    std::tie(x, y, z) = directionVector;

    std::tuple<float, float> attitude;

    pitch = atan2(z, sqrt(y * y + z * z)) / PI * 180;
    heading = atan2(y, z) / PI * 180;
    if (heading < 0)
        heading += 360;

    std::get<0>(attitude) = pitch;
    std::get<1>(attitude) = heading;
    return attitude;
}