#pragma once

#include <vector>

#include "basic.h"
#include "float.h"
#include "linalg/linalg.h"
using namespace linalg::aliases;

class AABB {
   public:
    double3 min;
    double3 max;

    // Compute the intersection of a ray with an AABB in the described depth interval.
    bool intersect(Ray ray, double t_min, double t_max);
};

// Retrieve the 8 corners of the AABB.
std::vector<double3> retrieve_corners(AABB aabb);

// Create an AABB given a list of points.
AABB construct_aabb(std::vector<double3> points);

// Combine two AABBs to build an AABB that encompasses both.
AABB combine(AABB a, AABB b);

// Determine if the lower corner of b is greater than a with respect to the specified axis.
bool compare(AABB a, AABB b, int axis);
