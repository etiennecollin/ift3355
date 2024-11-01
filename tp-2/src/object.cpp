#include "object.h"

// Fonction retournant soit la valeur v0 ou v1 selon le signe.
int rsign(double value, double v0, double v1) { return (int(std::signbit(value)) * (v1 - v0)) + v0; }

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection d'une sphère.
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Sphere::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    // Calculate the terms of the quadratic equation
    double a =
        ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y + ray.direction.z * ray.direction.z;
    double b = 2 * (ray.origin.x * ray.direction.x + ray.origin.y * ray.direction.y + ray.origin.z * ray.direction.z);
    double c = ray.origin.x * ray.origin.x + ray.origin.y * ray.origin.y + ray.origin.z * ray.origin.z - 1;

    // Check if the discriminant is negative
    double root = b * b - 4 * a * c;
    if (root < 0) return false;

    // Get the two intersections
    root = sqrt(root);
    double t_p = (-b + root) / (2 * a);
    double t_m = (-b - root) / (2 * a);

    // Check if both intersections are behind the camera
    if (t_p < 0 && t_m < 0) {
        return false;
    }

    // Get the closest intersection
    double t;
    if (t_p < 0 || t_m < 0) {
        t = fmax(t_p, t_m);
    } else {
        t = fmin(t_p, t_m);
    }

    // Check if the intersection is within the depth
    if (t < t_min || t > t_max) {
        return false;
    }

    hit->depth = t;
    hit->position = ray.origin + ray.direction * t;
    hit->key_material = this->key_material;
    hit->normal = linalg::normalize(hit->position);

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour la sphère.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Sphere::compute_aabb() { return Object::compute_aabb(); }

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un quad (rectangle).
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Quad::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    // 1. Trouver le plan du quad
    // 2. Trouver l'intersection du rayon avec le plan
    // 3. Vérifier si l'intersection est dans le quad
    // 4. Calculer les coordonnées UV
    // 5. Calculer la normale
    // 6. Remplir la structure Intersection
    // 7. Retourner vrai

    return false;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le quad (rectangle).
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Quad::compute_aabb() {
    return Object::compute_aabb();
    // return Object::compute_aabb();
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un cylindre.
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Cylinder::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) { return false; }

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le cylindre.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Cylinder::compute_aabb() { return Object::compute_aabb(); }

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un mesh.
//
// Référez-vous au PDF pour la paramétrisation pour les coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
//
bool Mesh::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) { return false; }

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un triangle.
// S'il y a intersection, remplissez hit avec l'information sur la normale et les coordonnées texture.
bool Mesh::intersect_triangle(Ray ray, double t_min, double t_max, Triangle const tri, Intersection *hit) {
    // Extrait chaque position de sommet des données du maillage.
    double3 const &p0 = positions[tri[0].pi];  // ou Sommet A (Pour faciliter les explications)
    double3 const &p1 = positions[tri[1].pi];  // ou Sommet B
    double3 const &p2 = positions[tri[2].pi];  // ou Sommet C

    // Triangle en question. Respectez la convention suivante pour vos variables.
    //
    //     A
    //    / \
	//   /   \
	//  B --> C
    //
    // Respectez la règle de la main droite pour la normale.

    // @@@@@@ VOTRE CODE ICI
    // Décidez si le rayon intersecte le triangle (p0,p1,p2).
    // Si c'est le cas, remplissez la structure hit avec les informations
    // de l'intersection et renvoyez true.
    // Pour plus de d'informations sur la géométrie, référez-vous à la classe dans object.hpp.
    //
    // NOTE : hit.depth est la profondeur de l'intersection actuellement la plus proche,
    // donc n'acceptez pas les intersections qui occurent plus loin que cette valeur.

    return false;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le Mesh.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Mesh::compute_aabb() { return Object::compute_aabb(); }
