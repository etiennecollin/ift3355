#pragma once

#include <vector>

#include "object.h"
#include "basic.h"
#include "aabb.h"

//Interface d'un container pour différente intersection.
class IContainer {
public:
    virtual ~IContainer() {};

    // Intersecte le rayon avec l'ensemble des objets dans l'intervalle spécifiée.
    // Retourne vrai s'il y a intersection sinon faux.
	virtual bool intersect(Ray ray, double t_min, double t_max, Intersection* hit) = 0;
};

// Structure contenant l'index et le AABB associé.
// Pratique pour créer l'algorithme de BVH.
struct BVHObjectInfo {
    // Index de l'objet
    int idx;

    // AABB associé à l'objet
    AABB aabb;
};

// Structure représentant chaque noeud dans l'arbre BVH
struct BVHNode {
    // Noeud de gauche
    BVHNode* left;
    // Noeud de droite
    BVHNode* right;

    // AABB englobant les deux neuds.
    AABB aabb;

    // Index de l'objet dans la liste
    int idx;
};

// Classe contenant la liste d'objet et la racine de l'arbre BVH.
class BVH : virtual public IContainer {
public:
    //Liste d'objets représentants tous les objets dans la scène.
    std::vector<Object*> objects;

    // Racine de l'arbre BVH
    // NOTE UTILE: Si les valeurs de left et right sont nulles, il s'agit d'une feuille.
    BVHNode* root;

    //Constructeur de BVH qui appelle récursivement recursive_build afin de construire l'arbre.
    BVH(std::vector<Object*> objs) : objects(objs) {
        std::vector<BVHObjectInfo> bvhs;

        for (int iobj = 0; iobj < objects.size(); iobj++) {
            bvhs.push_back({iobj, objects[iobj]->compute_aabb()});
        }

        root = recursive_build(bvhs, 0, bvhs.size(), 0);
    };
    ~BVH() {};

    //À adapter pour BVH
	bool intersect(Ray ray, double t_min, double t_max, Intersection* hit);
private:

    // Fonction recursive permettant la construction de notre arbre BVH
    // On choisit aléatoirement un axe. On trie la liste en fonction de l'axe.
    // On construit récursivement les autres noeuds également.
    // On combine le AABB des deux noeuds après récursions.
    BVHNode* recursive_build(std::vector<BVHObjectInfo> bvhs, int idx_start, int idx_end, int axis) {
        BVHNode* node = new BVHNode{};

        auto comparator = [=](BVHObjectInfo a, BVHObjectInfo b) {
            return compare(a.aabb,b.aabb,axis);
        };

        //s'il y a un seul élément, il s'agit d'une feuille. On arrête la récursion.
        if (idx_end - idx_start == 1){
            node->left = node->right = nullptr;
            node->idx = bvhs[idx_start].idx;
            node->aabb = bvhs[idx_start].aabb;
        }
        // sinon, on parcourt récursivement
        else {
            std::sort(bvhs.begin() + idx_start, bvhs.begin() + idx_end, comparator);

            int mid = idx_start + (idx_end - idx_start)/2;
            node->left = recursive_build(bvhs, idx_start, mid, (axis+1)%3);
            node->right = recursive_build(bvhs, mid, idx_end, (axis+1)%3);
            node->aabb = combine(node->left->aabb,node->right->aabb);
            node->idx = -1;
        }

        return node;
    };
};

class Naive : virtual public IContainer {
public:
    //Liste d'objets représentants tous les objets dans la scène.
    std::vector<Object*> objects;
    //Liste de AABB pour chaque objet.
    std::vector<AABB> aabbs;

    //Simple constructeur de Naive à partir d'une liste d'objets
    Naive(std::vector<Object*> objs) : objects(objs) {
        for (auto obj : objects) {
            aabbs.push_back(obj->compute_aabb());
        }
    }
    ~Naive() {};

    //À adapter pour Naive
	bool intersect(Ray ray, double t_min, double t_max, Intersection* hit);
};