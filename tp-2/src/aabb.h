#pragma once

#include <vector>

#include "float.h"
#include "basic.h"
#include "linalg/linalg.h"
using namespace linalg::aliases;

class AABB{
public:
    double3 min;
    double3 max;

    // Calcul l'intersection d'un rayon avec un AABB qui respecte l'intervalle de profondeur décrit.
    bool intersect(Ray ray, double t_min, double t_max);
};

// Retrouver les 8 coins associés au AABB.
std::vector<double3> retrieve_corners(AABB aabb);

// Construit un AABB à partir d'une série de points.
AABB construct_aabb(std::vector<double3> points);

// Combine deux AABB afin de construire un AABB qui englobe les deux.
AABB combine(AABB a, AABB b);

// Détermine si le coin inférieur de b est plus grand que a par rapport à l'axe spécifiée.
bool compare(AABB a, AABB b, int axis);