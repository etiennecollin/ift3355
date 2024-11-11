#pragma once

#include <vector>

#include "aabb.h"
#include "basic.h"
#include "object.h"

// Interface of a container for different intersection
class IContainer {
   public:
    virtual ~IContainer() {};

    // Intersect a ray with all objects in the specified interval
    // Returns true if there is an intersection otherwise false
    virtual bool intersect(Ray ray, double t_min, double t_max, Intersection* hit) = 0;
};

// Structure containing the index and associated AABB
// Convenient for creating the BVH algorithm
struct BVHObjectInfo {
    // Index de l'objet
    int idx;

    // AABB associé à l'objet
    AABB aabb;
};

// Structure representing each node in the BVH tree
struct BVHNode {
    // Left node
    BVHNode* left;
    // Right node
    BVHNode* right;

    // AABB encompassing the two nodes
    AABB aabb;

    // Index of the object in the list
    int idx;
};

// Class containing the list of objects and the root of the BVH tree
class BVH : virtual public IContainer {
   public:
    // List of objects representing all objects in the scene
    std::vector<Object*> objects;

    // Root of the BVH tree
    BVHNode* root;

    // BVH constructor that recursively calls recursive_build to build the tree
    BVH(std::vector<Object*> objs) : objects(objs) {
        std::vector<BVHObjectInfo> bvhs;

        // Vector to store futures for each AABB computation
        std::vector<std::future<BVHObjectInfo>> futures;

        // Launch asynchronous tasks to compute AABBs in parallel and create BVHObjectInfo instances
        for (int iobj = 0; iobj < objects.size(); iobj++) {
            futures.push_back(std::async(std::launch::async, [&, iobj] {
                // Assuming BVHObjectInfo can be constructed with (int, AABB)
                return BVHObjectInfo{iobj, objects[iobj]->compute_aabb()};
            }));
        }

        // Wait for each future to complete and collect the results in bvhs
        for (auto& future : futures) {
            bvhs.push_back(future.get());
        }

        root = recursive_build(bvhs, 0, bvhs.size(), 0);
    };
    ~BVH() {};

    // Adapted for BVH
    bool intersect(Ray ray, double t_min, double t_max, Intersection* hit);

   private:
    // Recursive function for building our BVH tree
    // Choose a random axis. Sort the list according to the axis
    // We recursively build the other nodes as well
    // We combine the AABB of the two nodes after recursions
    BVHNode* recursive_build(std::vector<BVHObjectInfo> bvhs, int idx_start, int idx_end, int axis) {
        BVHNode* node = new BVHNode{};

        auto comparator = [=](BVHObjectInfo a, BVHObjectInfo b) { return compare(a.aabb, b.aabb, axis); };

        // If there is only one element, it is a leaf. We stop the recursion
        if (idx_end - idx_start == 1) {
            node->left = node->right = nullptr;
            node->idx = bvhs[idx_start].idx;
            node->aabb = bvhs[idx_start].aabb;
        }
        // Else, we recusively explore the left and right nodes
        else {
            std::sort(bvhs.begin() + idx_start, bvhs.begin() + idx_end, comparator);

            int mid = idx_start + (idx_end - idx_start) / 2;
            node->left = recursive_build(bvhs, idx_start, mid, (axis + 1) % 3);
            node->right = recursive_build(bvhs, mid, idx_end, (axis + 1) % 3);
            node->aabb = combine(node->left->aabb, node->right->aabb);
            node->idx = -1;
        }

        return node;
    };
};

class Naive : virtual public IContainer {
   public:
    // List of the objects representing all of the scene objects
    std::vector<Object*> objects;
    // List of AABB for all objects
    std::vector<AABB> aabbs;

    // Simple constructor of Naive from a list of objects
    Naive(std::vector<Object*> objs) : objects(objs) {
        // Vector to store futures for each AABB computation
        std::vector<std::future<AABB>> futures;

        // Launch asynchronous tasks to compute AABBs in parallel
        for (auto obj : objects) {
            futures.push_back(std::async(std::launch::async, &Object::compute_aabb, obj));
        }

        // Wait for each future to complete and collect the results
        for (auto& future : futures) {
            aabbs.push_back(future.get());
        }
    }
    ~Naive() {};

    // Adapted for Naive
    bool intersect(Ray ray, double t_min, double t_max, Intersection* hit);
};
