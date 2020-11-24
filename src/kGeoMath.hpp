#pragma once
#include <tuple>
#include <eigen3/Eigen/Dense>

std::tuple<double, double, double> attitudeToDirection(Eigen::Vector2f attitude);

Eigen::Vector2f directionToAttitude(std::tuple<double, double, double> directionVector);

Eigen::Vector2f haversine(float latitude, float longitude);
