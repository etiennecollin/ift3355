#pragma once

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "basic.h"
#include "bitmap_image/bitmap_image.h"
#include "linalg/linalg.h"
using namespace linalg::aliases;
#include "aabb.h"

// The type of a "list of parameters", e.g. a map from strings to lists of numbers
typedef std::map<std::string, std::vector<double> > ParamList;

// A class to encapsulate all the parameters of a material
class Material {
   public:
    // Constructors
    Material() {};
    Material(bitmap_image& b, ParamList& params) { init(b, params); }

    void init(bitmap_image& b, ParamList& params) {
#define SET_BITMAP(_name) _name = b;
        SET_BITMAP(texture_albedo);

#define SET_VEC3(_name)                                                                                   \
    _name = params[#_name].size() == 3 ? double3{params[#_name][0], params[#_name][1], params[#_name][2]} \
                                       : double3{0, 0, 0};
        SET_VEC3(color_albedo);

#define SET_FLOAT(_name) _name = params[#_name].size() == 1 ? params[#_name][0] : 0;
        SET_FLOAT(k_ambient)
        SET_FLOAT(k_diffuse)
        SET_FLOAT(k_specular)
        SET_FLOAT(metallic)

        SET_FLOAT(shininess)
        SET_FLOAT(refractive_index)

        SET_FLOAT(k_reflection)
        SET_FLOAT(k_refraction)
    }
    // Texture of a material NON-normalized [r,g,b \in 0..=255]
    bitmap_image texture_albedo;

    // Color of a material normalized [r,g,b \in 0..=1] if no texture is present
    double3 color_albedo;

    // Coefficients that modulate the parameters of ambient, diffuse, and specular light
    double k_ambient;
    double k_diffuse;
    double k_specular;

    // Metallic coefficient for specular reflection [0 -> Metallic surface, 1 -> Plastic surface
    double metallic;

    // Shininess coefficient (Specular Exponent).
    double shininess;

    // Refractive index of the material [1 corresponds to the ambient air]
    double refractive_index;

    // Reflection coefficient of the color captured during ray casting
    double k_reflection;

    // Refraction coefficient of the color captured during ray casting
    double k_refraction;
};

// A class that encapsulates the information following an intersection
class Intersection {
   public:
    // Ray depth
    double depth;

    // Intersection position
    double3 position;

    // Normal at the intersection surface
    double3 normal;

    // UV coordinates associated with the intersection [between 0 and 1]
    double2 uv;

    // Key of the material used
    std::string key_material;

    // Store object id
    double obj_id;

    Intersection() : depth(DBL_MAX) {}
};

// Abstract base class for objects
class Object {
   public:
    double4x4 transform;    // Transformation from local to global space (local --> global
    double4x4 i_transform;  // Transformation from global to local space (global --> local)

    double3x3 n_transform;  // Transformation from local to global space for normals (local --> global) [3x3 matrix

    std::string key_material;  // Object material

    // Setup the 3 transformations from the given (global-to-object) transformation
    void setup_transform(double4x4 m) {
        transform = m;
        i_transform = inverse(m);
        n_transform = {{i_transform[0][0], i_transform[1][0], i_transform[2][0]},
                       {i_transform[0][1], i_transform[1][1], i_transform[2][1]},
                       {i_transform[0][2], i_transform[1][2], i_transform[2][2]}};
    };

    // Intersect the object with the given ray in global space.
    // Return true if there was an intersection with information about the intersection.
    bool intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
        // Local space ray
        Ray lray{mul(i_transform, {ray.origin, 1}).xyz(), mul(i_transform, {ray.direction, 0}).xyz()};

        if (local_intersect(lray, t_min, t_max, hit)) {
            hit->key_material = key_material;

            // Transform the intersection coordinates to global space.
            hit->position = mul(transform, {hit->position, 1}).xyz();
            hit->normal = normalize(mul(n_transform, hit->normal));

            return true;
        }

        return false;
    };

    // Construct the bounding box for the given object.
    // This must be called after the objects are formed and before the ray casting begins.
    // This must be done in GLOBAL space.
    virtual AABB compute_aabb() {
        AABB aabb;
        aabb.min = double3{-DBL_MAX, -DBL_MAX, -DBL_MAX};
        aabb.max = double3{DBL_MAX, DBL_MAX, DBL_MAX};

        return aabb;
    };

   protected:
    // Intersect the oblect with the given ray in local space.
    // This function is specific to each object subtype.
    // Returns true if there was an intersection, hit is then updated with the parameters.
    virtual bool local_intersect(Ray ray, double t_min, double t_max, Intersection* hit) = 0;
};

// Local space: Sphere centered at the origin with a radius (radius)
class Sphere : public Object {
   public:
    // Radius of the sphere
    double radius;

    Sphere(double r) : radius(r) {};

    // Compute the sphere's AABB
    virtual AABB compute_aabb();

   protected:
    // Intersect the sphere with the given ray in local space.
    virtual bool local_intersect(Ray ray, double t_min, double t_max, Intersection* hit);
};

// Local space: Quad(Rectangle) centered at the origin such that the normal is Z+ for a width of (2 * half_size) x (2 *
// half_size)
class Quad : public Object {
   public:
    // Half width
    double half_size;

    Quad(double s) : half_size(s) {};

    // Compute the quad's AABB
    virtual AABB compute_aabb();

   protected:
    // Intersect the quad with the given ray in local space.
    virtual bool local_intersect(Ray const ray, double t_min, double t_max, Intersection* hit);
};

// Local space: Cylinder such that the main axis is aligned with the Y-axis for a height (2 * half_height) with a radius
// (radius)
class Cylinder : public Object {
   public:
    // Radius of the cylinder
    double radius;
    // Half height of the cylinder relative to the origin.
    double half_height;

    Cylinder(double radius, double height) : radius(radius), half_height(height) {};

    // Compute the cylinder's AABB
    virtual AABB compute_aabb();

   protected:
    // Intersect the cylinder with the given ray in local space.
    virtual bool local_intersect(Ray ray, double t_min, double t_max, Intersection* hit);
};

// A class representing a vertex of a polygon.
// The stored integers are indices into the positions/tex_coords/normals vectors of the object the vertex belongs to.
class Vertex {
   public:
    // Indices into the positions, tex_coords, and normals vectors
    int pi, ti, ni;

    Vertex() : pi(-1), ti(-1), ni(-1) {}

    Vertex(int pi, int ti, int ni) : pi(pi), ti(ti), ni(ni) {}
};

// A triangle with 3 vertices containing the indices associated with the object
class Triangle {
   public:
    Vertex v[3];

    Triangle(Vertex const& v0, Vertex const& v1, Vertex const& v2) {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }

    Vertex& operator[](int i) { return v[i]; }
    const Vertex& operator[](int i) const { return v[i]; }
};

// Local space: Mesh centered at the origin with the specified positions, normals, and texture coordinates associated
// with each triangle
class Mesh : public Object {
   public:
    // Containers for positions, texture coordinates, normals, and colors. Look up by index
    std::vector<double3> positions;
    std::vector<double3> normals;
    std::vector<double2> tex_coords;

    // Triangles are triplets of vertices
    std::vector<Triangle> triangles;

    // List the OBJ data from a given file.
    Mesh(std::ifstream& file) {
        // Continues to retrieve the op code and parse it. We assume there is only one op per line
        while (file.good()) {
            std::string opString;
            std::getline(file, opString);

            std::stringstream opStream(opString);
            std::string opCode;
            opStream >> opCode;

            // Skip blank lines and comments
            if (!opCode.size() || opCode[0] == '#') {
                continue;
            }

            // Ignore groups
            if (opCode[0] == 'g' || opCode[0] == 'o' || opCode[0] == 's') {
                std::cerr << "ignored OBJ opCode '" << opCode << "'" << std::endl;
            }  // Vertex data
            else if (opCode[0] == 'v') {
                // Read 4 doubles at most
                std::vector<double> vec;
                for (int i = 0; opStream.good() && i < 3; i++) {
                    double v;
                    opStream >> v;
                    vec.push_back(v);
                }

                // Store this data in the right vector
                switch (opCode.size() > 1 ? opCode[1] : 'v') {
                    case 'v':
                        positions.push_back({vec[0], vec[1], vec[2]});
                        break;
                    case 't':
                        tex_coords.push_back({vec[0], vec[1]});
                        break;
                    case 'n':
                        normals.push_back({vec[0], vec[1], vec[2]});
                        break;
                    default:
                        std::cerr << "unknown vertex type '" << opCode << "'" << std::endl;
                        break;
                }
            }  // A polygon or a face
            else if (opCode == "f") {
                std::vector<Vertex> polygon;
                // limit the number of vertices to 4, since we only handle triangles or quads
                for (int i = 0; opStream.good() && i < 4; i++) {
                    // Get the full specification of a vertex
                    std::string vertexString;
                    opStream >> vertexString;

                    if (!vertexString.size()) {
                        break;
                    }

                    // Analyse the vertex into a set of indices for positions, texture coordinates, normals and colors,
                    // respectively
                    std::stringstream vertexStream(vertexString);
                    std::vector<int> indices;
                    for (int j = 0; vertexStream.good() && j < 3; j++) {
                        // Skip slashes
                        if (vertexStream.peek() == '/') {
                            vertexStream.ignore(1);
                        }
                        int index;
                        if (vertexStream >> index) indices.push_back(index);
                    }

                    // Transform the retrieved data into a real vertex, and add it to the polygon
                    if (indices.size()) {
                        indices.resize(3, 0);
                        polygon.push_back(Vertex(indices[0] - 1, indices[1] - 1, indices[2] - 1));
                    }
                }

                // We only accept triangles...
                if (polygon.size() == 3) {
                    triangles.push_back(Triangle(polygon[0], polygon[1], polygon[2]));
                }  // ... and quads ...
                else if (polygon.size() == 4) {
                    // ... but we split quads into two triangles
                    triangles.push_back(Triangle(polygon[0], polygon[1], polygon[2]));
                    triangles.push_back(Triangle(polygon[0], polygon[2], polygon[3]));
                }

                // All other op codes are ignored
            } else {
                std::cerr << "unknown opCode '" << opCode << "'" << std::endl;
            }
        }
    }

    // Compute the mesh's AABB
    virtual AABB compute_aabb();

   protected:
    // Intersect the mesh with the given ray in local space
    virtual bool local_intersect(Ray const ray, double t_min, double t_max, Intersection* hit);

    // Find the intersection point between the given ray and the triangular mesh.
    // Returns true if an intersection exists, and fills the data of the hit structure with the right information.
    bool intersect_triangle(Ray const ray, double t_min, double t_max, Triangle const tri, Intersection* hit);
};
