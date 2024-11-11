#include "object.h"

#include "linalg/linalg.h"

// Return either v0 or v1 depending on the sign of the value
int rsign(double value, double v0, double v1) { return (int(std::signbit(value)) * (v1 - v0)) + v0; }

// Intersection of a ray with a sphere
bool Sphere::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    // Calculate the terms of the quadratic equation
    double a = pow(ray.direction.x, 2) + pow(ray.direction.y, 2) + pow(ray.direction.z, 2);
    double b = 2 * (ray.origin.x * ray.direction.x + ray.origin.y * ray.direction.y + ray.origin.z * ray.direction.z);
    double c = pow(ray.origin.x, 2) + pow(ray.origin.y, 2) + pow(ray.origin.z, 2) - pow(this->radius, 2);

    // Check if the discriminant is negative
    double root = pow(b, 2) - 4 * a * c;
    if (root < 0) return false;

    // Get the two intersections
    root = sqrt(root);
    double division = 1 / (2 * a);
    double t_p = (-b + root) * division;
    double t_m = (-b - root) * division;

    // Check if both intersections are behind the camera
    if (t_p <= 0 && t_m <= 0) {
        return false;
    }

    // Check if the intersections is within the depth
    if ((t_p < t_min || t_p > t_max) && (t_m < t_min || t_m > t_max)) {
        return false;
    }

    // Get the closest intersection
    double t;
    bool is_tp_valid = (t_p > 0 && t_p >= t_min && t_p <= t_max);
    bool is_tm_valid = (t_m > 0 && t_m >= t_min && t_m <= t_max);
    if (is_tp_valid && is_tm_valid) {
        // Both intersections are within bounds, take the closest one
        t = fmin(t_p, t_m);
    } else if (is_tp_valid || is_tm_valid) {
        // Only one of the intersections is within bounds, take the valid one
        t = is_tp_valid ? t_p : t_m;
    } else {
        return false;
    }

    hit->depth = t;
    hit->key_material = this->key_material;
    hit->position = ray.origin + ray.direction * t;

    // Compute normal
    double3 normal = normalize(hit->position);
    if (dot(ray.direction, normal) > 0) {
        normal = -normal;
    }
    hit->normal = normal;

    // Compute UV coordinates
    // Precompute the inverse of pi
    double pi_inverse = 1 / M_PI;
    hit->uv = double2{0.5 + atan2(-normal.x, -normal.z) * pi_inverse / 2, 0.5 - asin(normal.y) * pi_inverse};

    return true;
}

// Compute the AABB for the sphere
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

// Compute the intersection of a ray with a quad
bool Quad::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    // Quick check if the ray is parallel to the xy plane
    if (ray.direction.z == 0) {
        return false;
    }

    // Compute x intersection of ray with xy plane
    double t = (-ray.origin.z) / ray.direction.z;

    // Check if the intersection is within the depth
    if (t < t_min || t > t_max || t <= 0) {
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

    // Compute UV coordinates
    // Precompute the inverse the length of the quad
    double inverse_length = 1 / (2 * this->half_size);
    hit->uv = (double2{intersection.x, -intersection.y} + this->half_size) * inverse_length;

    return true;
}

// Compute the AABB for the quad
AABB Quad::compute_aabb() {
    // Generate the 8 corners of the quad
    double epsilon = 1e-6;

    // Get the position of the quad
    std::vector<double3> points = {
        mul(this->transform, double4{this->half_size, this->half_size, epsilon, 1}).xyz(),
        mul(this->transform, double4{this->half_size, -this->half_size, epsilon, 1}).xyz(),
        mul(this->transform, double4{-this->half_size, this->half_size, epsilon, 1}).xyz(),
        mul(this->transform, double4{-this->half_size, -this->half_size, epsilon, 1}).xyz(),
        mul(this->transform, double4{this->half_size, this->half_size, -epsilon, 1}).xyz(),
        mul(this->transform, double4{this->half_size, -this->half_size, -epsilon, 1}).xyz(),
        mul(this->transform, double4{-this->half_size, this->half_size, -epsilon, 1}).xyz(),
        mul(this->transform, double4{-this->half_size, -this->half_size, -epsilon, 1}).xyz(),
    };
    return construct_aabb(points);
}

// Compute the intersection of a ray with a cylinder
bool Cylinder::local_intersect(Ray ray, double t_min, double t_max, Intersection *hit) {
    // Calculate the terms of the quadratic equation
    double a = pow(ray.direction.x, 2) + pow(ray.direction.z, 2);
    double b = 2 * (ray.origin.x * ray.direction.x + ray.origin.z * ray.direction.z);
    double c = pow(ray.origin.x, 2) + pow(ray.origin.z, 2) - pow(this->radius, 2);

    // ========================================================================
    // Check if the discriminant is negative
    double root = pow(b, 2) - 4 * a * c;
    if (root < 0) return false;

    // Get the two intersections
    root = sqrt(root);
    double division = 1 / (2 * a);
    double t_p = (-b + root) * division;
    double t_m = (-b - root) * division;

    // Check if both intersections are behind the camera
    if (t_p <= 0 && t_m <= 0) {
        return false;
    }

    // Check if the intersections is within the depth
    if ((t_p < t_min || t_p > t_max) && (t_m < t_min || t_m > t_max)) {
        return false;
    }

    // Get the closest valid intersection
    double3 hit_position_tp = ray.origin + ray.direction * t_p;
    double3 hit_position_tm = ray.origin + ray.direction * t_m;

    bool is_tp_valid = (t_p > 0 && t_p >= t_min && t_p <= t_max && hit_position_tp.y >= -this->half_height &&
                        hit_position_tp.y <= this->half_height);
    bool is_tm_valid = (t_m > 0 && t_m >= t_min && t_m <= t_max && hit_position_tm.y >= -this->half_height &&
                        hit_position_tm.y <= this->half_height);

    double t;
    double3 intersection;
    if (is_tp_valid && is_tm_valid) {
        // Both intersections are within bounds, take the closest one
        t = fmin(t_p, t_m);
        intersection = (t == t_p) ? hit_position_tp : hit_position_tm;
    } else if (is_tp_valid || is_tm_valid) {
        // Only one of the intersections is within bounds, take the valid one
        t = is_tp_valid ? t_p : t_m;
        intersection = is_tp_valid ? hit_position_tp : hit_position_tm;
    } else {
        return false;
    }

    hit->depth = t;
    hit->position = intersection;
    hit->key_material = this->key_material;

    // For a point on the cylinder surface, the normal is in the xz-plane
    double3 normal = normalize(double3{intersection.x, 0, intersection.z});

    // If the ray is hitting the inside of the cylinder, flip the normal to point outward
    if (dot(ray.direction, normal) > 0) {
        normal = -normal;
    }
    hit->normal = normal;

    // Compute UV coordinates
    // Precompute the inverse of pi
    double pi_inverse = 1 / M_PI;
    hit->uv = double2{this->radius + atan2(normal.x, normal.z) * pi_inverse / 2,
                      (-intersection.y + this->half_height) / (2 * this->half_height)};

    return true;
}

// Compute the AABB for the cylinder
AABB Cylinder::compute_aabb() {
    // Get 6 points on each side of the cylinder
    std::vector<double3> points = {
        mul(this->transform, double4{this->radius, this->half_height, this->radius, 1}).xyz(),
        mul(this->transform, double4{this->radius, this->half_height, -this->radius, 1}).xyz(),
        mul(this->transform, double4{this->radius, -this->half_height, this->radius, 1}).xyz(),
        mul(this->transform, double4{this->radius, -this->half_height, -this->radius, 1}).xyz(),
        mul(this->transform, double4{-this->radius, this->half_height, this->radius, 1}).xyz(),
        mul(this->transform, double4{-this->radius, this->half_height, -this->radius, 1}).xyz(),
        mul(this->transform, double4{-this->radius, -this->half_height, this->radius, 1}).xyz(),
        mul(this->transform, double4{-this->radius, -this->half_height, -this->radius, 1}).xyz(),
    };
    return construct_aabb(points);
}

// Compute the intersection of a ray with a mesh
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

// Compute the intersection of a ray with a triangle
bool Mesh::intersect_triangle(Ray ray, double t_min, double t_max, Triangle const tri, Intersection *hit) {
    // Compute the edges of the triangle
    double3 const &p0 = this->positions[tri[0].pi];
    double3 const &p1 = this->positions[tri[1].pi];
    double3 const &p2 = this->positions[tri[2].pi];
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
    if (t < t_min || t > t_max || t > hit->depth || t <= 0) {
        return false;
    }

    hit->depth = t;
    hit->position = ray.origin + t * ray.direction;

    double w = 1 - u - v;

    // Interpolate the normal
    double3 const &n0 = this->normals[tri[0].ni];
    double3 const &n1 = this->normals[tri[1].ni];
    double3 const &n2 = this->normals[tri[2].ni];
    hit->normal = normalize(w * n0 + u * n1 + v * n2);

    // Interpolate the uv coordinates for texture mapping
    double2 const &tex0 = this->tex_coords[tri[0].ti];
    double2 const &tex1 = this->tex_coords[tri[1].ti];
    double2 const &tex2 = this->tex_coords[tri[2].ti];
    hit->uv = w * tex0 + u * tex1 + v * tex2;

    return true;
}

// Compute the AABB for the mesh
AABB Mesh::compute_aabb() {
    std::vector<double3> positions = this->positions;

    // Apply transformation to each position
    for (int i = 0; i < positions.size(); i++) {
        positions[i] = mul(this->transform, double4{positions[i], 1}).xyz();
    }
    return construct_aabb(positions);
}
