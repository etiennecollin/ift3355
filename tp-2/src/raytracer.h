#pragma once

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "frame.h"
#include "linalg/linalg.h"
#include "resource_manager.h"
#include "scene.h"
using namespace linalg::aliases;

class Raytracer {
   public:
    // Render the given scene by raytracing.
    // Update the frame with the color and depth found.
    static void render(const Scene& scene, Frame* output);

   private:
    // Cast a ray in the scen while being responsible for intersection detection.
    // Allows recursive calls to complete reflection and refraction.
    //
    // Parameters
    //      scene: Scene in which the ray is launched
    //      ray: Current ray in the scene
    //      ray_depth: Recursion depth of the currently launched ray
    //      out_color: Color associated with the intersection
    //      out_z_depth: Depth of the closest intersection that acts as an upper bound
    static void trace(const Scene& scene, Ray ray, int ray_depth, double3* out_color, double* out_z_depth);

    // Compute the shading at the intersection with the geometry.
    // Responsible for local illumination as well as shadow generation in the scene.
    //
    // Parameters
    //      scene: Scene in which the ray is launched
    //      hit: Intersection information
    // Returns the calculated color at the intersection point.
    static double3 shade(const Scene& scene, Intersection hit);
};
