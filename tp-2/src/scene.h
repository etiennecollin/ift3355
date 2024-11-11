#pragma once

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "container.h"
#include "linalg/linalg.h"
#include "object.h"
#include "resource_manager.h"
using namespace linalg::aliases;

struct Camera {
    Camera(void)
        : fovy(45),
          aspect(1.0),
          z_near(1.0),
          z_far(10000.0),
          position(0.0, 0.0, 0.0),
          center(0.0, 0.0, 1.0),
          up(0.0, 1.0, 0.0),
          defocus_angle(0.0),
          focus_distance(0.0) {}

    double fovy;
    double aspect;

    double z_near;
    double z_far;
    double3 position;
    double3 center;
    double3 up;

    double defocus_angle;
    double focus_distance;
};

struct CameraOrthographic {
    CameraOrthographic(void) : origin(-1.0, 0.0, 0.0), lookAt(0.0, 0.0, 0.0), minPosition(-1.0, -1.0, -1.0) {}

    double3 origin;
    double3 lookAt;
    double3 minPosition;
};

// A class that encapsulates all the parameters of a spherical light.
// When radius = 0, it is a point light.
class SphericalLight {
   public:
    // Constructors
    SphericalLight();
    SphericalLight(double3 const& position, ParamList& params) : position(position) { init(params); }

    // Initialize the light attributes with the given parameter
    void init(ParamList& params) {
#define SET_VEC3(_name)                                                                                   \
    _name = params[#_name].size() == 3 ? double3{params[#_name][0], params[#_name][1], params[#_name][2]} \
                                       : double3{0, 0, 0};
        SET_VEC3(emission)
#define SET_FLOAT(_name) _name = params[#_name].size() == 1 ? params[#_name][0] : 0;
        SET_FLOAT(radius)
    }

    // Light position
    double3 position;

    // Emission
    double3 emission;

    // Spherecal radius of the light source
    double radius;
};

// A class that stores all the parameters, materials, and objects in a scene that we are trying to render
class Scene {
   public:
    // Resolution (width/height) of the output image, in pixels
    int resolution[2];

    // The number of rays to cast per pixel
    double samples_per_pixel;

    // The radius of the jittering region when randomly sampling
    double jitter_radius;

    // The maximum number of recursions possible
    int max_ray_depth;

    // The camera used during the rendering of the scene
    Camera camera;

    // Ambiant light vector of the scene
    double3 ambient_light;

    // List of spherical lights
    std::vector<SphericalLight> lights;

    // List of pointers to the objects in the scene.
    // Note that the Object class is abstract so the items will actually point to Spheres, Planes, Meshes, etc.
    IContainer* container;

    Scene() {
        resolution[0] = resolution[1] = 640;
        samples_per_pixel = 1;
        max_ray_depth = 0;
    }
};
