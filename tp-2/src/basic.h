#pragma once

#include "linalg/linalg.h"
using namespace linalg::aliases;

// Added for multi-threading
#include <future>

#define PI 3.14159265358979323846
#define EPSILON 1e-6
#define WORLD_IOR 1.0

// Ramdom value between [0,1)
static double rand_double() { return double(rand()) / double((RAND_MAX)); }

// Random value between [0,1] for a vector
static double2 rand_double2() { return double2{rand_double(), rand_double()}; }

// Valeur aléatoire à l'intérieur d'un disque.
static double2 random_in_unit_disk() {
    while (true) {
        auto p = (2.0 * rand_double2() - 1.0);
        if (length2(p) >= 1) continue;
        return p;
    }
}

// Convert radian to degree
static double rad2deg(double rad) { return rad * 360.0 / (2 * PI); }

// Convert degree to radian
static double deg2rad(double deg) { return deg * 2 * PI / 360.0; }

// A class representing a ray
class Ray {
   public:
    Ray() : origin(0, 0, 0), direction(0, 0, 0) {}
    Ray(double3 origin_, double3 direction_) : origin(origin_), direction(direction_) {}

    double3 origin;     // Ray origin
    double3 direction;  // Ray direction
    std::vector<double2> ior_stack = {{-1, WORLD_IOR}};
};
