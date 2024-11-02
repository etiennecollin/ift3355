#include "object.h"

#include "linalg/linalg.h"

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
    double a = pow(ray.direction.x, 2) + pow(ray.direction.y, 2) + pow(ray.direction.z, 2);
    double b = 2 * (ray.origin.x * ray.direction.x + ray.origin.y * ray.direction.y + ray.origin.z * ray.direction.z);
    double c = pow(ray.origin.x, 2) + pow(ray.origin.y, 2) + pow(ray.origin.z, 2) - pow(this->radius, 2);

    // Check if the discriminant is negative
    double root = b * b - 4 * a * c;
    if (root < 0) return false;

    // Get the two intersections
    root = sqrt(root);
    double division = 1 / (2 * a);
    double t_p = (-b + root) * division;
    double t_m = (-b - root) * division;

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
    hit->key_material = this->key_material;
    hit->position = ray.origin + ray.direction * t;
    hit->normal = normalize(hit->position);

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour la sphère.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Sphere::compute_aabb() {
    // Get 6 points; one for each face of the AABB
    double4 position = mul(this->transform, double4{0, 0, 0, 1});

    // Get get the position +- radius on each axis
    std::vector<double3> points = {
        double3{position.x + this->radius, position.y, position.z},
        double3{position.x - this->radius, position.y, position.z},
        double3{position.x, position.y + this->radius, position.z},
        double3{position.x, position.y - this->radius, position.z},
        double3{position.x, position.y, position.z + this->radius},
        double3{position.x, position.y, position.z - this->radius},

    };

    return construct_aabb(points);
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un quad (rectangle).
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Quad::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    // Quick check if the ray is parallel to the xy plane
    if (ray.direction.z == 0) {
        return false;
    }

    // Compute x intersection of ray with xy plane
    double t = (-ray.origin.z) / ray.direction.z;

    // Check if the intersection is within the depth
    if (t < t_min || t > t_max) {
        return false;
    }

    // Get the intersection point
    double3 intersection = ray.origin + ray.direction * t;

    // Check if the intersection is within the quad
    if (intersection.x < -this->half_size || intersection.x > this->half_size || intersection.y < -this->half_size ||
        intersection.y > this->half_size) {
        return false;
    }

    hit->depth = t;
    hit->key_material = this->key_material;
    hit->position = intersection;

    // The nomal of the hit should be z on the same direction as the ray
    hit->normal = ray.direction.z > 0 ? double3{0, 0, -1} : double3{0, 0, 1};

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le quad (rectangle).
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Quad::compute_aabb() { return Object::compute_aabb(); }

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
bool Mesh::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    bool did_hit = false;
    // Iterate over every triangle
    for (int i = 0; i < this->triangles.size(); i++) {
        Intersection local_hit;
        // Check if the ray intersects the triangle
        if (intersect_triangle(ray, t_min, t_max, this->triangles[i], &local_hit)) {
            if (i == 0 || local_hit.depth < hit->depth) {
                did_hit = true;
                *hit = local_hit;
            }
        }
    }

    return did_hit;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un triangle.
// S'il y a intersection, remplissez hit avec l'information sur la normale et les coordonnées texture.
//
bool Mesh::intersect_triangle(Ray ray, double t_min, double t_max, Triangle const tri, Intersection *hit) {
    // Compute the edges of the triangle
    double3 const &p0 = positions[tri[0].pi];
    double3 const &p1 = positions[tri[1].pi];
    double3 const &p2 = positions[tri[2].pi];
    double3 edge_1 = p1 - p0;
    double3 edge_2 = p2 - p0;

    // Convert to uvw coordinates
    // =========================================================================
    // Source: Möller–Trumbore Intersection Algorithm
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    // =========================================================================
    double3 ray_cross_edge = cross(ray.direction, edge_2);
    double similarity = dot(edge_1, ray_cross_edge);

    // If the ray is parallel to the triangle, the dot product between the ray and the normal will be 0
    if (similarity == 0) {
        return false;
    }

    // Precompute the division
    double division = 1 / similarity;

    // Compute the u parameter
    double3 point_ray_direction = ray.origin - p0;
    float u = dot(point_ray_direction, ray_cross_edge) * division;

    // If u is not within 0 and 1, the intersection is outside the triangle
    if (u < 0 || u > 1) {
        return false;
    }

    // Compute the v parameter
    double3 point_ray_cross_edge = cross(point_ray_direction, edge_1);
    double v = dot(ray.direction, point_ray_cross_edge) * division;

    // Check again if the intersection is outside the triangle
    if (v < 0 || u + v > 1) {
        return false;
    }

    // Get the intersection
    double t = dot(edge_2, point_ray_cross_edge) * division;
    // =========================================================================
    // End of Möller–Trumbore Intersection Algorithm
    // =========================================================================

    // Check if the intersection is within bounds
    if (t < t_min || t > t_max || t > hit->depth) {
        return false;
    }

    hit->depth = t;
    hit->position = ray.origin + t * ray.direction;

    double w = 1 - u - v;

    // Interpolate the normal
    double3 const &n0 = normals[tri[0].ni];
    double3 const &n1 = normals[tri[1].ni];
    double3 const &n2 = normals[tri[2].ni];
    hit->normal = normalize(w * n0 + u * n1 + v * n2);

    // Interpolate the uv coordinates for texture mapping
    double2 const &tex0 = tex_coords[tri[0].ti];
    double2 const &tex1 = tex_coords[tri[1].ti];
    double2 const &tex2 = tex_coords[tri[2].ti];
    hit->uv = w * tex0 + u * tex1 + v * tex2;

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le Mesh.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Mesh::compute_aabb() { return construct_aabb(positions); }
