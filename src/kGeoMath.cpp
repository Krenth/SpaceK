#include <tuple>
#include <cmath>
#include <boost/geometry.hpp>
#include <eigen3/Eigen/Dense>
#include "kGeoMath.hpp"

#define PI 3.14159265

namespace bg = boost::geometry;

std::tuple<double, double, double> attitudeToDirection(Eigen::Vector2f attitude)
{
    bg::model::point<long double, 2, bg::cs::spherical<bg::degree>> dir_spherical(90 - attitude(1), 90 - attitude(0));
    bg::model::point<long double, 3, bg::cs::cartesian> dir_cartesian;
    bg::transform(dir_spherical, dir_cartesian);
    std::tuple<double, double, double> directionVector;
    std::get<0>(directionVector) = bg::get<2>(dir_cartesian);
    std::get<1>(directionVector) = bg::get<1>(dir_cartesian);
    std::get<2>(directionVector) = bg::get<0>(dir_cartesian);
    return directionVector;
}

Eigen::Vector2f directionToAttitude(std::tuple<double, double, double> directionVector)
{
    double x, y, z;
    float pitch, heading;

    std::tie(x, y, z) = directionVector;

    Eigen::Vector2f attitude;

    pitch = atan2(z, sqrt(y * y + z * z)) / PI * 180;
    heading = atan2(y, z) / PI * 180;
    if (heading < 0)
        heading += 360;

    attitude(0) = pitch;
    attitude(1) = heading;

    return attitude;
}


Eigen::Vector2f haversine(float latitude, float longitude)
{   
    Eigen::Vector2f distVector;
    return distVector;
}