#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <cfloat>

#include "scene.h"
#include "frame.h"
#include "resource_manager.h"
#include "linalg/linalg.h"
using namespace linalg::aliases;

class Raytracer 
{
public:
    // Rend la scène donnée par lancer de rayon.
    // Met à jour la frame avec la couleur et la profondeur trouvée.
    static void render(const Scene& scene, Frame *output);

private:
    // Lance un rayon dans la scène tout en étant responsable de la détection d'intersection.
    // Permet des appels récursifs pour compléter la réflection et la réfraction.
    // 
    // Paramètres
    //   scene: Scène dans laquelle le rayon est lancé
    //   rayon: Rayon actuel dans la scène
    //   ray_depth: Profondeur de récursion du rayon actuellement lancé
    //   out_color: Couleur associée à l'intersection
    //   out_z_depth: Profondeur de la plus proche intersection qui agit comme une borne supérieure

    static void trace(const Scene& scene, 
                      Ray ray, int ray_depth, 
                      double3 *out_color, double *out_z_depth);

    // Calcule l'ombrage (le shading) à l'intersection avec la géométrie.
    // Responsable de l'illumination locale ainsi que de la génération des ombres dans la scène.
    // 
    // Paramètres
    //   scene: Scène dans laquelle le rayon est lancé
    //   hit: Information sur l'intersection
    //
    // Renvoie la couleur calculée au point d'intersection.
	static double3 shade(const Scene& scene,
                        Intersection hit);
};