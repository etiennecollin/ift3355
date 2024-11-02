#include "aabb.h"

// @@@@@@ VOTRE CODE ICI
// Implémenter l'intersection d'un rayon avec un AABB dans l'intervalle décrit.
// Source: Pete Shirley's blog, 2016/02/14
// https://psgraphics.blogspot.com/2016/02/new-simple-ray-box-test-from-andrew.html
bool AABB::intersect(Ray ray, double t_min, double t_max) {
    // If we are within the min and max for all x, y, z coordinates, we are inside the AABB
    // Check all 3 dimensions
    for (int i = 0; i < 3; i++) {
        // Isolate t in the equation aabb_min = ray_o + ray_d * t
        double division = 1 / ray.direction[i];
        double t0 = (this->min[i] - ray.origin[i]) * division;
        double t1 = (this->max[i] - ray.origin[i]) * division;

        // In case the min is on the opposite side of the AABB,
        // then the ray origin is closer to the AABB max than min point.
        // We need to swap t0 and t1 to have the right vector direction
        if (ray.direction[i] < 0) {
            std::swap(t0, t1);
        }

        // Update the t_min and t_max
        t_min = fmax(t_min, t0);
        t_max = fmin(t_max, t1);

        // Check that the hit is within the bounds
        if (t_max < t_min) {
            return false;
        }
    }

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Implémenter la fonction qui permet de trouver les 8 coins de notre AABB.
std::vector<double3> retrieve_corners(AABB aabb) {
    return std::vector<double3>{
        double3{aabb.min[0], aabb.min[1], aabb.min[2]}, double3{aabb.min[0], aabb.min[1], aabb.max[2]},
        double3{aabb.min[0], aabb.max[1], aabb.min[2]}, double3{aabb.min[0], aabb.max[1], aabb.max[2]},
        double3{aabb.max[0], aabb.min[1], aabb.min[2]}, double3{aabb.max[0], aabb.min[1], aabb.max[2]},
        double3{aabb.max[0], aabb.max[1], aabb.min[2]}, double3{aabb.max[0], aabb.max[1], aabb.max[2]}};
};

// @@@@@@ VOTRE CODE ICI
// Implémenter la fonction afin de créer un AABB qui englobe tous les points.
AABB construct_aabb(std::vector<double3> points) {
    // Get min and max in xyz
    double3 min = double3{DBL_MAX, DBL_MAX, DBL_MAX};
    double3 max = double3{-DBL_MAX, -DBL_MAX, -DBL_MAX};

    // Iterate through all points
    for (int i = 0; i < points.size(); i++) {
        // Update min and max
        for (int j = 0; j < 3; j++) {
            min[j] = fmin(min[j], points[i][j]);
            max[j] = fmax(max[j], points[i][j]);
        }
    }

    return AABB{min, max};
}

AABB combine(AABB a, AABB b) { return AABB{min(a.min, b.min), max(a.max, b.max)}; };

bool compare(AABB a, AABB b, int axis) { return a.min[axis] < b.min[axis]; };
