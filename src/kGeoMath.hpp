#pragma once
#include <tuple>

std::tuple<double, double, double> attitudeToDirection(float pitch, float heading);

std::tuple<float, float> directionToAttitude(std::tuple<double, double, double> directionVector);
