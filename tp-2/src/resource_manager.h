#pragma once
#include "object.h"

class ResourceManager {
   public:
    // Initialise the instance
    static ResourceManager* Instance();

    // Release the instance
    static void Release();

    // All the different materials are kept here for performance
    std::map<std::string, Material> materials;

   private:
    static ResourceManager* Instance_;

    ResourceManager();

    ~ResourceManager();
};
