#include "container.h"

// - Use DFS to search the tree
//      - If it is a leaf, intersect with the geometry
//      - Otherwise, it is an internal node
//          - Intersect the ray with the left and right AABB
//              - If there is an intersection, add the node to the stack
// - Return the intersection with the smallest maximum depth
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
                        did_hit = true;
                        *hit = local_hit;
                        hit->obj_id = currentNode->idx;
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

// - Iterate over all of the objects
//      - Detect intersection with the AABB
//      - If intersection, detect intersection with the geometry
//          - If intersection, update the parameters
// - Return the intersection with the smallest maximum depth
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
                    hit->obj_id = i;
                }
            }
        }
    }

    return did_hit;
}
