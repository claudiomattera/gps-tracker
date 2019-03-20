#include "distance.h"

#include <cmath>

namespace {

double const EARTH_RADIUS = 6370.693486;

inline double degree_to_radian(double x)
{
    return (x * M_PI) / 180.0;
}

} // unnamed namespace

double compute_distance(double latitude_1, double longitude_1, double latitude_2, double longitude_2)
{
    double const theta = degree_to_radian(longitude_1 - longitude_2);

    latitude_1 = degree_to_radian(latitude_1);
    latitude_2 = degree_to_radian(latitude_2);

    double const distance = EARTH_RADIUS * acos(
        (sin(latitude_1) * sin(latitude_2)) + (cos(latitude_1) * cos(latitude_2) * cos(theta))
    );
    return distance;
}
