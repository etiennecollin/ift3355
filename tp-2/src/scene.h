#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <cfloat>

#include "resource_manager.h"
#include "object.h"
#include "container.h"
#include "linalg/linalg.h"
using namespace linalg::aliases;

struct Camera
{
    Camera(void) : fovy(45), aspect(1.0), z_near(1.0), z_far(10000.0),
        position(0.0, 0.0, 0.0), center(0.0, 0.0, 1.0), up(0.0, 1.0, 0.0), defocus_angle(0.0), focus_distance(0.0) {}

    //Comme décrit dans les notes
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

struct CameraOrthographic
{
    CameraOrthographic(void) : origin(-1.0, 0.0, 0.0), lookAt(0.0, 0.0, 0.0), minPosition(-1.0, -1.0, -1.0) {}

    //Comme décrit dans les notes
    double3 origin;
    double3 lookAt;
    double3 minPosition;


};

// Une classe pour encapsuler tous les paramètres d'une lumière sphèrique.
// Lorsque radius = 0, il s'agit d'une lumière ponctuelle.
class SphericalLight
{
public:
    // Constructeurs
    SphericalLight();
    SphericalLight(double3 const& position, ParamList& params) : position(position) { init(params); }

    // Initialise les attributs de la lumière avec la liste des paramètres données.
    void init(ParamList& params)
    {
#define SET_VEC3(_name) _name = params[#_name].size() == 3 ? double3{params[#_name][0],params[#_name][1],params[#_name][2]} : double3{0,0,0};
        SET_VEC3(emission)
#define SET_FLOAT(_name) _name = params[#_name].size() == 1 ? params[#_name][0] : 0;
            SET_FLOAT(radius)
    }

    // Position de la lumière.
    double3 position;

    // Emission 
    double3 emission;

    // Taille Sphérique de la source de lumière
    double radius;
};


// Une classe qui stocke tous les paramètres, matériaux et objets
// dans une scène que l'on cherche à rendre.
class Scene {
public:

    // Résolution (largeur/hauteur) de l'image de sortie, en pixels.
    int resolution[2];

    // Le nombre de rayon à lancer par pixel
    double samples_per_pixel;

    // Région de variation lors du sampling aléatoirement
    double jitter_radius;

    //Le nombre maximal de récursion possible.
    int max_ray_depth;

    // La caméra utilisée durant le rendu de la scène.
    Camera camera;

    // Vecteur correspondant à la lumière ambiante de la scène
    double3 ambient_light;

    // Liste des lumières sphériques.
    std::vector<SphericalLight> lights;

    // Liste des pointeurs vers les objets de la scène.
    // Notez que la classe Object est abstraite, donc les items pointeront réellement
    // vers des objets Spheres, Planes, Mehses, etc.
    IContainer* container;

    Scene()
    {
        resolution[0] = resolution[1] = 640;
        samples_per_pixel = 1;
        max_ray_depth = 0;
    }
};