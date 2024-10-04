#pragma once

#include "linalg/linalg.h"
using namespace linalg::aliases;

#define PI 3.14159265358979323846
#define EPSILON 1e-6

// Valeur aléatoire entre [0,1)
static double rand_double() {
	return double(rand()) / double((RAND_MAX));
}

// Valeur aléatoire entre [0,1] pour un vecteur
static double2 rand_double2() {
	return double2{rand_double(),rand_double()};
}

// Valeur aléatoire à l'intérieur d'un disque.
static double2 random_in_unit_disk() {
    while (true) {
        auto p = (2.0 * rand_double2() - 1.0);
        if (length2(p) >= 1) continue;
        return p;
    }
}

// Convertir radian vers degrée
static double rad2deg(double rad) {
	return rad * 360.0 / (2 * PI);
}

// Convertir degrée vers radian
static double deg2rad(double deg) {
	return deg * 2 * PI / 360.0;
}

// Une classe qui représente un rayon
class Ray 
{
public:
	Ray() : origin(0, 0, 0), direction(0, 0, 0) {}
	Ray(double3 origin_, double3 direction_) :
		origin(origin_), direction(direction_)
	{

	}

	double3 origin;    // Origine du rayon
	double3 direction; // Direction du rayon
};