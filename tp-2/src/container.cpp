#include "container.h"

// @@@@@@ VOTRE CODE ICI
// - Parcourir l'arbre DEPTH FIRST SEARCH selon les conditions suivantes:
//      - S'il s'agit d'une feuille, faites l'intersection avec la géométrie.
//      - Sinon, il s'agit d'un noeud altérieur.
//          - Faites l'intersection du rayon avec le AABB gauche et droite.
//              - S'il y a intersection, ajouter le noeud à ceux à visiter.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool BVH::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
    bool did_hit = false;
    if (root->aabb.intersect(ray, t_min, t_max)) {
        // Create a stack of BVHNodes* and initialize it with the root of the BVH
        std::vector<BVHNode*> stack;
        stack.push_back(root);

        Intersection local_hit;
        // Non-recursive DFS
        while (!stack.empty()) {
            BVHNode* currentNode = stack.back();
            stack.pop_back();
            if (currentNode->aabb.intersect(ray, t_min, t_max) && !currentNode->left && !currentNode->right) {
                if (objects[currentNode->idx]->intersect(ray, t_min, t_max, &local_hit)) {
                    if (local_hit.depth < hit->depth) {
                        *hit = local_hit;
                        did_hit = true;
                    }
                }
            } else if (currentNode->aabb.intersect(ray, t_min, t_max)) {
                if (currentNode->left) {
                    stack.push_back(currentNode->left);
                }
                if (currentNode->right) {
                    stack.push_back(currentNode->right);
                }
            }
        }
    }

    return did_hit;
}

// @@@@@@ VOTRE CODE ICI
// - Parcourir tous les objets
//      - Détecter l'intersection avec l'AABB
//      - Si intersection, détecter l'intersection avec la géométrie.
//          - Si intersection, mettre à jour les paramètres.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool Naive::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
    // Iterate through the objects
    std::vector aabbs = Naive::aabbs;
    std::vector objects = Naive::objects;
    bool did_hit = false;

    for (int i = 0; i < objects.size(); i++) {
        Intersection local_hit;
        // Detect intersection with AABB
        if (aabbs[i].intersect(ray, t_min, t_max)) {
            // Detect intersection with geometry
            if (objects[i]->intersect(ray, t_min, t_max, &local_hit)) {
                // Only update hit if it is the first hit or if it is closer
                // than the previous closest hit
                if (i == 0 || local_hit.depth < hit->depth) {
                    did_hit = true;
                    *hit = local_hit;
                }
            }
        }
    }

    return did_hit;
}
