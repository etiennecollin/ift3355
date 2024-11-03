#include "container.h"
#include <bits/stdc++.h>
using namespace std;

// @@@@@@ VOTRE CODE ICI
// - Parcourir l'arbre DEPTH FIRST SEARCH selon les conditions suivantes:
//      - S'il s'agit d'une feuille, faites l'intersection avec la géométrie.
//      - Sinon, il s'agit d'un noeud altérieur.
//          - Faites l'intersection du rayon avec le AABB gauche et droite.
//              - S'il y a intersection, ajouter le noeud à ceux à visiter.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool BVH::intersect(Ray ray, double t_min, double t_max, Intersection* hit) { 
    bool didHit=false;
    if(root->aabb.intersect(ray,t_min,t_max)){
        //crée une stack de BVHNodes* et l'initialise avec la root du BVH
        stack<BVHNode*> liste;
        liste.push(root);
        Intersection localHit;
        localHit.depth=DBL_MAX;
        //DFS non-récursif
        while(!liste.empty()){
            BVHNode* currentNode = liste.top();
            liste.pop();
            if(currentNode->aabb.intersect(ray,t_min,t_max) && !currentNode->left && !currentNode->right){
                if(objects[currentNode->idx]->intersect(ray,t_min,t_max,&localHit)){
                    if(localHit.depth<hit->depth){
                        *hit=localHit;
                        didHit=true;
                    }
                }
            }
            else if(currentNode->aabb.intersect(ray,t_min,t_max)){
                if(currentNode->left){
                    liste.push(currentNode->left);
                }
                if(currentNode->right){
                    liste.push(currentNode->right);
                }
            }
        }
    }
    
    return didHit; }

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
