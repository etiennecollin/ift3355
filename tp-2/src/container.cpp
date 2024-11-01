#include "container.h"

// @@@@@@ VOTRE CODE ICI
// - Parcourir l'arbre DEPTH FIRST SEARCH selon les conditions suivantes:
//      - S'il s'agit d'une feuille, faites l'intersection avec la géométrie.
//      - Sinon, il s'agit d'un noeud altérieur.
//          - Faites l'intersection du rayon avec le AABB gauche et droite.
//              - S'il y a intersection, ajouter le noeud à ceux à visiter.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool BVH::intersect(Ray ray, double t_min, double t_max, Intersection* hit) { return true; }

// @@@@@@ VOTRE CODE ICI
// - Parcourir tous les objets
//      - Détecter l'intersection avec l'AABB
//      - Si intersection, détecter l'intersection avec la géométrie.
//          - Si intersection, mettre à jour les paramètres.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool Naive::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
    // Iterate through the objects
    std::vector objects = Naive::objects;
    bool did_hit = false;

    for (int i = 0; i < objects.size(); i++) {
        AABB aabb = Naive::aabbs[i];
        Intersection local_hit;
        // Detect intersection with AABB
        if (aabb.intersect(ray, t_min, t_max)) {
            // Detect intersection with geometry
            if (objects[i]->intersect(ray, t_min, t_max, &local_hit)) {
                // Only update hit if it is the first hit or if it is closer
                // than the previous hit and is not behind the camera
                if ((i == 0 || local_hit.depth < hit->depth) && local_hit.depth >= 0) {
                    did_hit = true;
                    hit = &local_hit;
                }
            }
        }
    }

    return did_hit;
}
